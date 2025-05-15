#ifndef include_ndi_command_handler_hpp
#define include_ndi_command_handler_hpp

#include <sdkddkver.h>

#include <deque>
#include <map>
#include <mutex>
#include <thread>

//#include <boost/asio/io_context.hpp>
#include <boost/crc.hpp>

#include "serial_port.hpp"

#include "ndi_types.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_base.hpp"

namespace device {

  namespace ndi {

    template <typename command_type>
    struct post_command
    {
      static void exec(serial_port* port, command_base<command_type>& com)
      {

      }
    };

    class command
    {
    public:
      command(serial_port* arg) : serial_port(arg) {}

      template <typename command_type>
      typename traits::tag<command_type>::result_type operator()(command_base<command_type>& com) const
      {
        std::unique_lock<std::mutex> lock(mutex);
        send<command_type>(serial_port, com);
        serial_port->set_timeout(traits::timeout<command_type>::value());
        auto rep = recv<command_type>(serial_port);
        post_command<command_type>::exec(serial_port, com);
        return read<command_type>(rep, com);
      }


    private:
      mutable std::mutex mutex;
      mutable serial_port* serial_port;
    };
  }
}

#endif
