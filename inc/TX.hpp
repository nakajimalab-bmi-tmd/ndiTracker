#ifndef include_ndi_command_TX_hpp
#define include_ndi_command_TX_hpp

#include <bitset>
#include <queue>

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct TX : command_base < TX >
    {
      TX() {}
      TX(tracking_mode mode) : mode_(mode) {}
      tracking_mode mode_;
    };

    namespace traits
    {
      template<>
      struct tag < TX >
      {
        typedef ascii_reply_tag reply_format;
        typedef tracking_data result_type;
      };

      template<>
      struct timeout<TX>
      {
        static constexpr uint32_t value()
        {
          return 1000;
        }
      };

      template<>
      struct send < TX >
      {
        static inline void exec(serial_port* serial_port, TX& com)
        {
          ndi::send(serial_port, "TX:%04X", com.mode_);
        }
      };

      template<>
      struct read < TX >
      {
        typedef tag<TX>::result_type result_type;
        static result_type exec(std::deque<char>& rep, TX& com);
      };
    }
  }
}

#endif
