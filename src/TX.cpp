#include <sdkddkver.h>
#include <boost/lexical_cast.hpp>

#include "ndi_types.hpp"
#include "TX.hpp"

namespace device {
  namespace ndi {
    namespace {

      std::string make_substr(std::queue<char>& src, size_t byte)
      {
        std::string rst;
        for (size_t i = 0; i < byte; ++i)
        {
          rst.push_back(src.front());
          src.pop();
        }
        return rst;
      }

      class handle
      {
      public:
        handle(tracking_mode const& mode, std::queue<char>& reply) : mode(mode), input(reply) {}
        handle_data get_handle_data();

      private:
        void read_transform_data(handle_data& data);
        tool_marker_information read_tool_marker_information();
        single_active_stray_marker read_single_active_marker();
        marker_position read_marker_on_tool();

        std::queue<char>& input;
        tracking_mode mode;
        handle_data data;
      };

      handle_data handle::get_handle_data()
      {
        try
        {
          handle_data h_data;

          // read reply option 0x0001
          if (mode.is_transformation_data_on())
          {
            read_transform_data(h_data);
          }

          if (!h_data.status.is_disabled())
          {
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
          if (input.front() != '\n')
          {
            throw exception(error_code::invalid_reply);
          }
          input.pop();
          return h_data;
        }
        catch (std::out_of_range e)
        {
          throw exception(error_code::invalid_reply);
        }
      }

      marker_position read_marker_position(std::queue<char>& input, bool all_transformation)
      {

        try
        {
          marker_position data;
          data.num_marker = hex_converter<uint16_t>(input, 2);
          if (data.num_marker == 0) return data;

          uint16_t num_of_chars_for_outofvolume = (data.num_marker + 3) / 4;
          uint16_t num_out_of_volume = 0;

          data.out_of_volume = boost::dynamic_bitset<uint8_t>(num_of_chars_for_outofvolume * 4);
          for (uint16_t i = 0; i < num_of_chars_for_outofvolume; ++i)
          {
            std::bitset<4> four_bits(hex_converter<uint16_t>(input, 1));
            for (uint16_t j = 0; j < 4; j++)
            {
              data.out_of_volume.set(4 * (num_of_chars_for_outofvolume - i - 1) + j, four_bits[j]);
            }
          }

          // read marker position
          uint16_t num_reported_marker = all_transformation ? data.num_marker : data.num_marker - static_cast<uint8_t>(data.out_of_volume.count());

          for (int i = 0; i < num_reported_marker; ++i)
          {
            std::array<float, 3> pos;
            for (int j = 0; j < 3; ++j)
            {
              pos[j] = hex_converter<int>(input, 7) * 0.01f;
            }
            data.positions.push_back(pos);
          }
          return data;
        }
        catch (boost::bad_lexical_cast e)
        {
          throw exception(error_code::invalid_reply);
        }
      }

      void handle::read_transform_data(handle_data& data)
      {
        assert(mode.is_transformation_data_on());

        try
        {
          if (input.front() == 'D' && make_substr(input, 8) == "DISABLED")
          {
            data.status.set_disabled();
          }
          else
          {
            if (input.front() == 'M' && make_substr(input, 7) == "MISSING")
            {
              data.status.set_missing();
            }
            else
            {
              data.status.set_valid();
              data.transform_data.data[0] = boost::lexical_cast<int>(make_substr(input, 6)) * 0.0001f;
              data.transform_data.data[1] = boost::lexical_cast<int>(make_substr(input, 6)) * 0.0001f;
              data.transform_data.data[2] = boost::lexical_cast<int>(make_substr(input, 6)) * 0.0001f;
              data.transform_data.data[3] = boost::lexical_cast<int>(make_substr(input, 6)) * 0.0001f;

              data.transform_data.data[4] = boost::lexical_cast<int>(make_substr(input, 7)) * 0.01f;
              data.transform_data.data[5] = boost::lexical_cast<int>(make_substr(input, 7)) * 0.01f;
              data.transform_data.data[6] = boost::lexical_cast<int>(make_substr(input, 7)) * 0.01f;
              data.transform_data.error = boost::lexical_cast<float>(make_substr(input, 6)) * 0.0001f;
            }

            // port status
            data.transform_data.status = port_status(hex_converter<uint32_t>(input, 8));
            data.transform_data.frame = hex_converter<uint32_t>(input, 8);
          }
        }
        catch (boost::bad_lexical_cast e)
        {
          throw exception(error_code::invalid_reply);
        }
      }

      tool_marker_information handle::read_tool_marker_information()
      {
        try
        {
          tool_marker_information info;
          info.tool = tool_information(static_cast<uint8_t>(hex_converter<uint16_t>(input, 2)));
          std::array<uint8_t, 10> bytes;
          for (int i = 0; i < 10; ++i)
          {
            bytes[i] = static_cast<uint8_t>(hex_converter<uint16_t>(input, 2));
          }
          info.marker = marker_information(bytes);
          return info;
        }
        catch (boost::bad_lexical_cast e)
        {
          throw exception(error_code::invalid_reply);
        }
      }

      single_active_stray_marker handle::read_single_active_marker()
      {
        assert(mode.is_single_stray_active_marker_on());

        try
        {
          single_active_stray_marker data;
          data.status = static_cast<uint8_t>(hex_converter<uint16_t>(input, 2));

          if (data.status.is_exist())
          {
            // not missing, or out of volume with reply option 0x0800
            if (!data.status.is_missing() || (data.status.is_out_of_volume() && mode.is_all_transformation_on()))
            {
              for (int i = 0; i < 3; ++i)
              {
                data.position[i] = boost::lexical_cast<float>(make_substr(input, 7)) * 0.01f;
              }
            }
          }
          return data;
        }
        catch (boost::bad_lexical_cast e)
        {
          throw exception(error_code::invalid_reply);
        }
      }

      marker_position handle::read_marker_on_tool()
      {
        return marker_position();// read_marker_position(input, mode.is_all_transformation_on());
      }

    } // namespace

    namespace traits {

      read<TX>::result_type read<TX>::exec(std::deque<char>& rep, TX& com)
      {
        std::queue<char> queue(std::move(rep));
        tracking_data data;

        // num handles
        uint16_t num_handle = hex_converter<uint16_t>(queue, 2);

        for (uint16_t i = 0; i < num_handle; ++i)
        {
          uint16_t port_handle = hex_converter<uint16_t>(queue, 2);
          handle h(com.mode_, queue);
          data.tools.emplace(port_handle, h.get_handle_data());
        }

        if (com.mode_.is_stray_passive_markers_on())
        {
          data.stray_markers = read_marker_position(queue, com.mode_.is_all_transformation_on());
        }
        return data;
      }

    }
  }
}
