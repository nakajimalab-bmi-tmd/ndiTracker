#ifndef include_ndi_tracker_hpp
#define include_ndi_tracker_hpp

#include <thread>

#include "ndi_command_handler.hpp"

#include "BX.hpp"
#include "COMM.hpp"
#include "INIT.hpp"
#include "RESET.hpp"
#include "PHSR.hpp"
#include "PENA.hpp"
#include "PHINF.hpp"
#include "PINIT.hpp"
#include "TSTART.hpp"
#include "TSTOP.hpp"
#include "TX.hpp"
#include "VER.hpp"

namespace device {
  namespace ndi {

    template <typename Device>
    class tracker;

    template <typename Device>
    class tracker_tool 
    {   
      friend class tracker<Device>;
    public:
      struct handle_information
      {
        std::wstring main_type;
        std::wstring manufacture_id;
        std::wstring tool_revision;

        std::wstring part_name;
      };

      tracker_tool() {}
      handle_data const& get_handle_data() const
      {
        return tool_data;
      }

      handle_information const& get_handle_information() const
      {
        return handle_information_;
      }

    protected:
      handle_data tool_data;
      handle_information handle_information_;
    };

    // to typedef 'tool_type' for tracker_tool derived struct for the Device
    template <typename Device>
    struct tracker_tool_tag 
    {
      // using tool_type = ;
    };

    template <typename Device>
    class tracker
    {
    public:
      inline Device& cast()
      {
        return *static_cast<Device*>(this);
      }

      tracker() : serial(new serial_port(io_context)), handler(serial.get())
      {

      }

      ~tracker() {}

      void auto_connect()
      {
          auto ports = serial_port::get_active_ports();
          auto it = std::find_if(ports.begin(), ports.end(), [](port_description const& description)
              {
                  return description.device_desc.find(Device::usb_port_name(), 0) != std::string::npos;
              }
          );

          if (it == ports.end())
          {
              throw std::runtime_error("no ndi port found.");
          }

          this->connect(it->com_port);
      }

      void connect(std::string const& port_name)
      {
        serial->open(port_name);
        ndi::post_command<COMM<Device>>::exec(serial.get(), COMM<Device>());

        try
        {
          this->handler(INIT());
        }
        catch (boost::system::system_error&)
        {
          // would be wrong COMM parameter
          this->handler(RESET());
          this->handler(COMM<Device>());
          this->handler(INIT());
        }

        this->info = this->handler(VER());
      }

      std::string const& get_device_name() const
      {
        return static_cast<Device const*>(this)->get_device_name();
      }

      void reset_system()
      {
        this->handler(RESET());
        this->handler(COMM<Device>());
        this->info = this->handler(VER());
        this->handler(INIT());
      }

      void detach_tracker_tool(tracker_tool<Device>&)
      {

      }

      void start(bool use_thread = true)
      {
        if (!is_tracking_)
        {
          data_for_write = &data[0];
          data_for_read = &data[1];
          this->handler(TSTART());
          is_tracking_ = true;
          if (use_thread) thread = std::thread(&tracker::tracking, this);
        }
      }

      void stop()
      {
        if (is_tracking_)
        {
          this->is_tracking_ = false;
          if (this->thread.get_id() != std::thread::id()) this->thread.join();
          this->handler(TSTOP());
        }
      }

      inline bool is_connected() const
      {
        return this->serial->is_open();
      }

      inline bool is_tracking() const
      {
        return is_tracking_;
      }

      std::map<uint16_t, typename tracker_tool_tag<Device>::tool_type> const& update_tracker_tools()
      {
        if (!is_tracking_) throw std::runtime_error("Not tracking");
        for (auto const& tool : this->data_for_read->tools)
        {
          try
          {
            uint16_t const& handle = tool.first;
            this->tracker_tools[handle].tool_data = tool.second;
          }
          catch (std::out_of_range&) {}
        }
        return this->tracker_tools;
      }

      typename tracker_tool_tag<Device>::tool_type const& get_tracker_tool(uint16_t const port)
      {
          return this->tracker_tools.at(port);
      }

      template<typename command_type>
      typename traits::tag<command_type>::result_type post_command(command_base<command_type>& com)
      {
        return this->handler(com);
      }

      void tracking()
      {
          tracking_mode mode;
          mode.all_transformation_on();
        while (this->is_tracking_)
        {
          //std::unique_lock<std::mutex> lock(mutex);
          *(this->data_for_write) = this->handler(BX(mode));
          std::swap(data_for_write, data_for_read);
        }
      }

      void tracking_one()
      {
          tracking_mode mode;
          mode.all_transformation_on();
          *(this->data_for_write) = this->handler(BX(mode));
          std::swap(data_for_write, data_for_read);
      }

      inline version_information const& get_version_information() const
      {
        return info;
      }

    protected:
      static boost::asio::io_context io_context;

      std::unique_ptr<serial_port> serial;
      command handler;

      std::map<uint16_t, typename tracker_tool_tag<Device>::tool_type> tracker_tools;

      bool is_tracking_ = false;
      std::thread thread;
      std::mutex mutable mutex;
      ndi::tracking_data data[2];
      ndi::tracking_data* data_for_write;
      ndi::tracking_data* data_for_read;
      version_information info;
    };

    template <typename Device>
    boost::asio::io_context tracker<Device>::io_context;
  }
}

#endif
