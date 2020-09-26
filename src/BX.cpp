#include <sdkddkver.h>

#include <queue>

#include "BX.hpp"
#include "binary_converter.hpp"

namespace device {
  namespace ndi {
    namespace {

      marker_position read_marker_position(std::queue<char>& reply, bool all_transformation)
      {
        marker_position data;

        // number of markers
        if (reply.size() < 1)
        {
          throw exception(error_code::invalid_reply);
        }
        data.num_marker = reply.front();
        reply.pop();

        // out of volume bit field
        uint16_t num_bytes_out_of_volume = (data.num_marker + 8 - 1) / 8;
        if (reply.size() < num_bytes_out_of_volume)
        {
          throw exception(error_code::invalid_reply);
        }

        for (int i = 0; i < num_bytes_out_of_volume; ++i)
        {
          data.out_of_volume.append(reply.front());
          reply.pop();
        }

        // read marker position
        uint16_t num_reported_marker = all_transformation ? data.num_marker : data.num_marker - static_cast<uint8_t>(data.out_of_volume.count());

        if (reply.size() < num_reported_marker * 4 * 3)
        {
          throw exception(error_code::invalid_reply);
        }

        for (int i = 0; i < num_reported_marker; ++i)
        {
          std::array<char, 4 * 3> bytes;
          for (int j = 0; j < 12; ++j)
          {
            bytes[j] = reply.front();
            reply.pop();
          }

          auto pos = binary_cast<std::array<float, 3>>(bytes.begin());
          data.positions.emplace_back(pos);
        }
        return data;
      }

      class handle
      {
      public:
        handle(tracking_mode const& mode, std::queue<char>& reply);

        handle_data get_handle_data();

      private:
        transform_data read_transform_data(handle_status const& status);
        tool_marker_information read_tool_marker_information();
        single_active_stray_marker read_single_active_marker();
        marker_position read_marker_on_tool();

        std::queue<char>& input;
        tracking_mode mode;
        handle_data data;
      };

      handle::handle(tracking_mode const& mode, std::queue<char>& reply) : input(reply), mode(mode)
      {
      }

      handle_data handle::get_handle_data()
      {
        handle_data h_data;

        // handle status
        handle_status stat(input.front()); input.pop();
        h_data.status = stat;

        if (!stat.is_disabled())
        {
          // read reply option 0x0001
          if (mode.is_transformation_data_on())
          {
            auto t_data = read_transform_data(stat);
            h_data.transform_data = t_data;
          }

          // read reply option 0x0002
          if (mode.is_tool_marker_information_on())
          {
            h_data.tool_marker_information = read_tool_marker_information();
          }

          /// read reply option 0x0004
          if (mode.is_single_stray_active_marker_on())
          {
            h_data.single_active_stray_marker = read_single_active_marker();
          }

          // read reply option 0x0008
          if (mode.is_markers_on_tool_on())
          {
            h_data.tool_markers = read_marker_on_tool();
          }
        }

        return h_data;
      }

      transform_data handle::read_transform_data(handle_status const& status)
      {
        assert(mode.is_transformation_data_on());
        transform_data transform_data;

        // read transform if valid
        if (status.is_valid())
        {
          if (input.size() < 32)
          {
            throw exception(error_code::invalid_reply);
          }

          std::array<char, 32> bytes;
          for (int i = 0; i < 32; ++i)
          {
            bytes[i] = input.front();
            input.pop();
          }

          transform_data.data = binary_cast<std::array<float, 8>>(bytes.begin());
        }

        // read port status and frames
          {
            if (input.size() < 8)
            {
              throw exception(error_code::invalid_reply);
            }

            std::array<char, 8> bytes;
            for (int i = 0; i < 8; ++i)
            {
              bytes[i] = input.front();
              input.pop();
            }

            auto data = binary_cast<std::array<uint32_t, 2>>(bytes.begin());

            transform_data.status = port_status(data[0]);
            transform_data.frame = data[1];
          }

          return transform_data;
      }

      tool_marker_information handle::read_tool_marker_information()
      {
        assert(mode.is_tool_marker_information_on());
        if (input.size() < 11) throw exception(error_code::invalid_reply);

        // tool information (1 byte)
        tool_marker_information tool_marker_info;
        tool_marker_info.tool = tool_information(input.front());
        input.pop();

        // marker information (10 bytes)
        std::array<uint8_t, 10> bytes;
        for (int i = 0; i < 10; ++i)
        {
          bytes[i] = input.front();
          input.pop();
        }
        tool_marker_info.marker = marker_information(bytes);

        return tool_marker_info;
      }

      single_active_stray_marker handle::read_single_active_marker()
      {
        assert(mode.is_single_stray_active_marker_on());

        if (input.size() < 1)
        {
          throw exception(error_code::invalid_reply);
        }

        single_active_stray_marker rst;
        single_active_stray_marker_status status(input.front());
        rst.status = status;
        input.pop();

        if (status.is_exist())
        {
          // not missing, or out of volume with reply option 0x0800
          if (!status.is_missing() || (status.is_out_of_volume() && mode.is_all_transformation_on()))
          {
            if (input.size() < 12)
            {
              throw exception(error_code::invalid_reply);
            }
            std::array<char, 12> bytes;
            for (int i = 0; i < 12; ++i)
            {
              bytes[i] = input.front();
              input.pop();
            }
            rst.position = binary_cast<std::array<float, 3>>(bytes.begin());
          }
        }

        return rst;
      }

      marker_position handle::read_marker_on_tool()
      {
        return read_marker_position(input, mode.is_all_transformation_on());
      }

    }


    namespace traits {
      /**
      /*  <# of Handles><Handle 1><Reply Opt 0001 Data>...<Reply Opt 0008 Data><LF>
      /*  ...
      /*  <Handle n><Reply Option 0001 Data>...<Reply Option 0008 Data><LF>
      /*  <Reply Option 1000 Data><System Status><CRC16><CR>                         **/
      read<BX>::result_type read<BX>::exec(std::deque<char>& rep, BX& com)
      {
        result_type data;
        std::queue<char> binary(std::move(rep));

        uint8_t num_handle = binary.front(); binary.pop();

        for (uint8_t i = 0; i < num_handle; ++i)
        {
          uint8_t port_handle = binary.front(); binary.pop();
          handle h(com.mode_, binary);
          data.tools.emplace(port_handle, h.get_handle_data());
        }

        if (com.mode_.is_stray_passive_markers_on())
        {
          data.stray_markers = read_marker_position(binary, com.mode_.is_all_transformation_on());
        }

        return data;
      }
    }
  }
}

