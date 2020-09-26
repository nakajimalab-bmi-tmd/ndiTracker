#ifndef include_ndi_command_INIT_hpp
#define include_ndi_command_INIT_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct INIT : command_base < INIT > {};

    namespace traits
    {
      template<>
      struct tag < INIT >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct send < INIT >
      {
        static inline void exec(serial_port* serial_port, INIT& com)
        {
          ndi::send(serial_port, "INIT:");
        }
      };
    }

  }
}

#endif
