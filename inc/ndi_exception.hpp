#ifndef include_ndi_exception_hpp
#define include_ndi_exception_hpp

#include <exception>
#include <cstdint>
#include <string>

namespace device {
  namespace ndi {

    enum class error_code
    {
      invalid_parameter,
      serial_error,
      invalid_reply,
      unable_send_command,
      crc_failure,
      error_returned,
    };

    class exception : public std::exception
    {
      std::string message;
      uint16_t device_code;
      error_code desc;
    public:
      exception(uint16_t device_err_code) : desc(error_code::error_returned), device_code(device_err_code) {}
      exception(error_code ec) : desc(ec), device_code(0x00) {}
      virtual const char* what() const;

      uint16_t code() const { return device_code; }
    };
  }
}

#endif
