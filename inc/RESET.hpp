#ifndef include_RESET_hpp
#define include_RESET_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct RESET : command_base < RESET > {};

    namespace traits
    {
      template<>
      struct tag < RESET >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct timeout<RESET>
      {
        static constexpr uint32_t value()
        {
          return 5000;
        }
      };

      template<>
      struct send < RESET >
      {
        static inline void exec(serial_port* serial_port, RESET& com)
        {
          serial_port->send_break();
          serial_port->set_baud_rate(9600);
          serial_port->set_flow_control(serial_port::flow_control::none);
        }
      };

      template <>
      struct read < RESET >
      {
        typedef void result_type;
        static inline result_type exec(std::deque<char>& reply, command_base<RESET>&)
        {
          std::string message(reply.begin(), reply.end());
          if (message == "RESET") return;
          throw std::runtime_error("Invalid reply");
        }
      };
    }
  }
}
#endif
