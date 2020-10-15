#ifndef include_ndi_command_PINIT_hpp
#define include_ndi_command_PINIT_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct PINIT : command_base < PINIT >
    {
      uint16_t port_handle_;
      explicit PINIT(uint16_t port_handle) : port_handle_(port_handle) {}
    };

    namespace traits
    {
      template<>
      struct tag < PINIT >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct timeout<PINIT>
      {
        static constexpr uint32_t value()
        {
          return 5000;
        }
      };

      template<>
      struct send < PINIT >
      {
        static inline void exec(serial_port* serial_port, PINIT& com)
        {
          ndi::send(serial_port, "PINIT:%02X", com.port_handle_);
        }
      };

    }

  }
}

#endif
