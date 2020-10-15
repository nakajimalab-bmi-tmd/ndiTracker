#ifndef __ml_tracking_ndi_PHRQ_hpp
#define __ml_tracking_ndi_PHRQ_hpp

#include <bitset>
#include <map>
#include <queue>
#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct PHSR : command_base < PHSR > {};

    namespace traits
    {
      template<>
      struct tag < PHSR >
      {
        typedef ascii_reply_tag reply_format;
        typedef std::map<uint16_t, port_status> result_type;
      };

      template<>
      struct send < PHSR >
      {
        static inline void exec(serial_port* serial_port, PHSR& com)
        {
          ndi::send(serial_port, "PHSR:");
        }
      };

      template<>
      struct read < PHSR >
      {
        typedef tag<PHSR>::result_type result_type;

        static inline result_type exec(std::deque<char>& rep, PHSR&)
        {
          if (rep.size() < 2) throw exception(error_code::invalid_reply);

          result_type res;

          std::queue<char> queue(std::move(rep));
          uint16_t const num_port_handle = hex_converter<uint16_t>(queue, 2);

          for (int i = 0; i < num_port_handle; ++i)
          {
            res.emplace(hex_converter<uint16_t>(queue, 2), port_status(hex_converter<uint16_t>(queue, 3)));
          }

          return res;
        }
      };
    }
  }
}

#endif
