#ifndef include_ndi_command_TSTART_hpp
#define include_ndi_command_TSTART_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct TSTART : command_base < TSTART > {};

    namespace traits
    {
      template<>
      struct tag < TSTART >
      {
        typedef ascii_reply_tag reply_format;
        typedef void result_type;
      };

      template<>
      struct timeout<TSTART>
      {
        static constexpr uint32_t value()
        {
          return 8000;
        }
      };

      template<>
      struct send < TSTART >
      {
        static inline void exec(serial_port* serial_port, TSTART& com)
        {
          ndi::send(serial_port, "TSTART:");
        }
      };
    }
  }
}

#endif
