#ifndef include_ndi_command_PENA_hpp
#define include_ndi_command_PENA_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct PENA : command_base < PENA >
    {
      enum tool_tracking_priority
      {
        STATIC = 'S',
        DYNAMIC = 'D',
        BUTTON = 'B'
      };

      uint16_t port_handle_;
      tool_tracking_priority priority_;
      explicit PENA(uint16_t port_handle, tool_tracking_priority priority = DYNAMIC) : port_handle_(port_handle), priority_(priority) {}
    };

    namespace traits
    {
      template<>
      struct tag < PENA >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct send < PENA >
      {
        static inline void exec(serial_port* serial_port, PENA& com)
        {
          ndi::send(serial_port, "PENA:%02X%c", com.port_handle_, com.priority_);
        }
      };

    }

  }
}

#endif
