#include <sdkddkver.h>
#include <boost/crc.hpp>
#include "binary_converter.hpp"
#include "serial_port.hpp"
#include "ndi_command_io.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace device {
  namespace ndi {

    void send(serial_port* serial_port, char* fmt, ...)
    {
      va_list argp;
      va_start(argp, fmt);
      auto len = _vscprintf(fmt, argp);
      std::unique_ptr<char[]> command(new char[len + 6]); // command(len) + crc(4) + \r + \0
      vsprintf_s(command.get(), len + 1, fmt, argp); // command(len) + \0
      va_end(argp);

      boost::crc_16_type crc16;
      crc16.process_bytes(command.get(), len);
      sprintf_s(&command.get()[len], 6, "%04X\r", crc16.checksum());

//#ifdef _DEBUG
//      std::cout << "<--" << command.get() << std::endl;
//#endif

      serial_port->write(command.get());
    }

    namespace traits {

      template<typename tag>
      void check_crc(std::deque<char>& reply);

      template<>
      void check_crc<ascii_reply_tag>(std::deque<char>& reply)
      {
        uint16_t expected_crc;
        boost::crc_16_type crc16;
        size_t i = 0;
        auto it = reply.begin();
        for (; i < reply.size() - 4; ++i, ++it)
        {
          crc16.process_byte(reply[i]);
        }

        char* end;
        std::string attached_crc(it, reply.end());
        expected_crc = static_cast<uint16_t>(strtoul(attached_crc.c_str(), &end, 16));

        if (*end != 0) throw std::runtime_error("invalid_reply");

        if (crc16.checksum() != expected_crc)
        {
          throw std::runtime_error("crc_failure");
        }

        // remove crc
        for (int i = 0; i < 4; ++i) reply.pop_back();
      }

      template<>
      void check_crc<binary_reply_tag>(std::deque<char>& reply)
      {
        boost::crc_16_type crc16;
        size_t i = 0;
        auto it = reply.begin();
        for (; i < reply.size() - 2; ++i, ++it)
        {
          crc16.process_byte(reply[i]);
        }

        uint16_t attached_crc = binary_cast<uint16_t>(it);
        if (crc16.checksum() != attached_crc)
        {
          throw std::runtime_error("crc_failure");
        }
      }

      std::deque<char> recv<ascii_reply_tag>::exec(serial_port* serial_port)
      {
        auto reply = serial_port->read_until<std::deque>('\r');
        reply.pop_back();
        check_crc<ascii_reply_tag>(reply);

#ifdef _DEBUG
        std::cout << "-->" << std::string(reply.begin(), reply.end()) << std::endl;
#endif
        return reply;
      }

      std::deque<char> recv<binary_reply_tag>::exec(serial_port* serial_port)
      {
        std::deque<char> reply = serial_port->read<std::deque>(6);
        if (binary_cast<uint16_t>(reply.begin()) != 0xA5C4)
        {
          reply = serial_port->read<std::deque>(2041);
          check_crc<ascii_reply_tag>(reply);
          return reply;
        }

        check_crc<binary_reply_tag>(reply);
        uint16_t length = binary_cast<uint16_t>(reply.begin() + 2);

        reply = serial_port->read<std::deque>(length + 2);

        check_crc<binary_reply_tag>(reply);

        //for (int i = 0; i < 6; ++i) reply.pop_front();
        //for (int i = 0; i < 2; ++i) reply.pop_back();
        return reply;
      }

      template struct recv < ascii_reply_tag > ;
      template struct recv < binary_reply_tag > ;
    }

    namespace detail
    {
      bool is_okay(std::string const& message)
      {
        return (message.size() == 4 && message == "OKAY");
      }

      void check_error(std::string const& message)
      {
        if (message.size() == 7 && message.substr(0, 5) == "ERROR")
        {
          char* end;
          uint16_t code = static_cast<uint16_t>(strtoul(message.substr(5, 2).c_str(), &end, 16));
          throw std::runtime_error("Error");
        }
      }

      bool is_warning(std::string const& message)
      {
        static const std::map<uint16_t, std::string> warning_string_map =
        {
          { 0x00, "A non-fatal tool error has been encountered" },
          { 0x02, "The tool you are trying to enable is a unique geometry tool that doesnÅft meet the unique geometry requirements." },
          { 0x03, "The tool you are trying to enable is a unique geometry tool that conflicts with another unique geometry tool already loaded and enabled." },
          { 0x04, "The tool you are trying to enable is a unique geometry tool that doesnÅft meet the unique geometry requirements, and conflicts with another unique geometry tool already loaded and enabled." },
          { 0x05, "The system has selected a default marker wavelength to track a tool." }
        };

        uint16_t code = 0xFF;
        if (message.size() == 7 && message == "WARNING")
        {
          code = 0;
        }
        else if (message.size() == 9 && message.substr(0, 7) == "WARNING")
        {
          char* end;
          code = static_cast<uint16_t>(strtoul(message.substr(7, 2).c_str(), &end, 16));

          if (*end != 0) throw std::runtime_error("Warning");
        }
        else
        {
          throw std::runtime_error("Invalid Reply");
        }
        return true;
      }
    }
  }
}

/*
*/
