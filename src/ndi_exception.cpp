#include "ndi_exception.hpp"

#include <map>
#include <sstream>

namespace device {
  namespace ndi {

    const std::map<uint8_t, std::string> ndi_error_map = {
      { 0x01, "Invalid Command" },
      { 0x02, "Command too long" },
      { 0x03, "Command too short" },
      { 0x04, "Invalid CRC" },
      { 0x05, "Timeout on command execution" },
      { 0x06, "Unable to set up new communication parameters" },
      { 0x07, "Incorrect number of parameters" },
      { 0x08, "Invalid port handle selected" },
      { 0x09, "Invalid mode selected" },
      { 0x0A, "Invalid LED selected" },
      { 0x0B, "Invalid LED state selected" },
      { 0x0C, "Command is invalid while in the current mode" },
      { 0x0D, "No tool is assigned to the selected port handle" },
      { 0x0E, "Selected port handle not initialized" },
      { 0x0F, "Selected port handle not enabled" },
      { 0x10, "System not initialized" },
      { 0x11, "Unable to stop tracking" },
      { 0x12, "Unable to start tracking" },
      { 0x13, "Hardware error" },
      { 0x14, "Invalid Position Sensor characterization paramters" },
      { 0x15, "Unable to initialize the system" },
      { 0x16, "Unable to start Diagnostic mode" },
      { 0x17, "Unable to stop Diagnostic mode" },
      { 0x19, "Unable to read device's firmware version information" },
      { 0x1A, "Internal system error" },
      { 0x1C, "Unable to set marker activation signature" },
      { 0x1D, "Unable to find SROM device IDs" },
      { 0x1E, "Unable to read SROM device data" },
      { 0x1F, "Unable to write SROM device data" },
      { 0x20, "Unable to select SROM device for given port handle and SROM device ID" },
      { 0x21, "Unable to test electrical current on tool" },
      { 0x22, "Enabled tools are not supported by selected volume parameters" },
      { 0x23, "Command parameter is out of range" },
      { 0x24, "Unable to select measurement volume" },
      { 0x25, "Unable to determine the systemÅfs supported features list" },
      { 0x28, "Too many tools are enabled" },
      { 0x2A, "No memory is available for dynamic allocation" },
      { 0x2B, "The requested port handle has not been allocated" },
      { 0x2C, "The requested port handle has become unoccupied" },
      { 0x2D, "All handles have been allocated" },
      { 0x2E, "Incompatible firmware versions" },
      { 0x2F, "Invalid port description" },
      { 0x30, "Requested port is already assigned a port handle" },
      { 0x31, "Invalid input or output state" },
      { 0x32, "Invalid operation for the device associated with the specified port handle" },
      { 0x33, "Feature not available" },
      { 0x34, "User parameter does not exist" },
      { 0x35, "Invalid value type" },
      { 0x36, "User parameter value set is out of valid range" },
      { 0x37, "User parameter array index is out of valid range" },
      { 0x38, "User parameter size is incorrect" },
      { 0x39, "Permission denied" },
      { 0x3B, "File not found" },
      { 0x3C, "Error writing to file" },
      { 0x40, "Tool definition file error" },
      { 0x41, "Tool characteristics not supported" },
      { 0x42, "Device not present" }
    };

    const std::map<error_code, std::string> description = {
      { error_code::invalid_parameter, "Invalid parameter" },
      { error_code::serial_error, "Serial error" },
      { error_code::invalid_reply, "Invalid reply" },
      { error_code::unable_send_command, "Unable to send command" },
      { error_code::crc_failure, "CRC failure" },
      { error_code::error_returned, "Error was returned" }
    };

    const char* exception::what() const
    {
      std::ostringstream oss;
      oss << description.at(desc);

      if (desc == error_code::error_returned)
      {
        oss << ": Error" << device_code;

        auto it = ndi_error_map.find(static_cast<uint8_t>(device_code));
        if (it != ndi_error_map.end())
        {
          oss << " (" << it->second << ")";
        }
        else
        {
          oss << " (Unknown)";
        }
      }

      const_cast<std::string&>(message) = oss.str();
      return message.c_str();
    }

  }
}
