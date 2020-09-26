#ifndef include_ndi_command_io_hpp
#define include_ndi_command_io_hpp

#include <cstdint>
#include <deque>
#include <map>
#include <queue>

#include "ndi_command_tag.hpp"
#include "ndi_command_base.hpp"
#include "ndi_exception.hpp"

namespace device {

  class serial_port;

  namespace ndi {

    template<typename T>
    T hex_converter(std::queue<char>& src, size_t byte)
    {
      std::unique_ptr<char[]> data(new char[byte + 1]);
      for (size_t i = 0; i < byte; ++i)
      {
        data[i] = src.front();
        src.pop();
      }
      data[byte] = 0;

      char* end;
      T rst = static_cast<T>(std::strtoul(data.get(), &end, 16));

      if (*end != 0) throw std::bad_cast();
      return rst;
    }

    namespace detail {

      bool is_okay(std::string const& message);
      void check_error(std::string const& message);
      bool is_warning(std::string const& message);

    } //namespace detail

    namespace traits {

      template <typename command>
      struct send
      {
        static std::deque<char> exec(serial_port*, command_base<command>&)
        {
          static_assert(false, "invalid command");
        }
      };

      template <typename format_tag>
      struct recv
      {
        static std::deque<char> exec(serial_port* serial_port)
        {
          static_assert(false, "invalid reply format tag");
        }
      };

      template <>
      struct recv < ascii_reply_tag >
      {
        static std::deque<char> exec(serial_port* serial_port);
      };

      template <>
      struct recv < binary_reply_tag >
      {
        static std::deque<char> exec(serial_port* serial_port);
      };

      template <typename command>
      struct read
      {
        typedef void result_type;
        static inline result_type exec(std::deque<char>& reply, command_base<command>&)
        {
          std::string message(reply.begin(), reply.end());
          if (detail::is_okay(message)) return;
          detail::check_error(message);
          if (detail::is_warning(message)) return;
          throw std::runtime_error("Invalid Reply");
        }
      };



    } //namespace traits

    namespace dispatch {

      template <typename command>
      struct recv
      {
        static std::deque<char> exec(serial_port* serial_port)
        {
          return traits::recv<traits::tag<command>::reply_format>::exec(serial_port);
        }
      };

    } //namespace dispatch

    template <typename command>
    void send(serial_port* serial_port, command_base<command>& com)
    {
      traits::send<command>::exec(serial_port, static_cast<command&>(com));
    }

    void send(serial_port* serial_port, char* fmt, ...);

    template <typename command>
    std::deque<char> recv(serial_port* serial_port)
    {
      return dispatch::recv<command>::exec(serial_port);
    }

    template<typename command>
    typename traits::read<command>::result_type read(std::deque<char>& reply, command_base<command>& com)
    {
      return traits::read<command>::exec(reply, static_cast<command&>(com));
    }
  }
}

#endif
