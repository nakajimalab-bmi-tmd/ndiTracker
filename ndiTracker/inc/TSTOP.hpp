#ifndef include_ndi_command_TSTOP_hpp
#define include_ndi_command_TSTOP_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct TSTOP : command_base < TSTOP > {};

    namespace traits
    {
      template<>
      struct tag < TSTOP >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct send < TSTOP >
      {
        static inline void exec(serial_port* serial_port, TSTOP& com)
        {
          ndi::send(serial_port, "TSTOP:");
        }
      };
    }
  }
}

#endif
