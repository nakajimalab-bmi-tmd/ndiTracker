#ifndef include_ndi_command_BX_hpp
#define include_ndi_command_BX_hpp

#include <bitset>
#include <queue>

#include "serial_port.hpp"
#include "ndi_types.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct BX : command_base < BX >
    {
      BX() {}
      BX(tracking_mode mode) : mode_(mode) {}
      tracking_mode mode_;
    };

    namespace traits
    {
      template<>
      struct tag < BX >
      {
        typedef binary_reply_tag reply_format;
        typedef tracking_data result_type;
      };

      template<>
      struct timeout<BX>
      {
        static constexpr uint32_t value()
        {
          return 1000;
        }
      };

      template<>
      struct send < BX >
      {
        static inline void exec(serial_port* serial_port, BX& com)
        {
          ndi::send(serial_port, "BX:%04X", com.mode_);
        }
      };

      template<>
      struct read < BX >
      {
        typedef tag<BX>::result_type result_type;
        static result_type exec(std::deque<char>& rep, BX& com);
      };
    }
  }
}

#endif
