#ifndef include_ndi_command_VER_hpp
#define include_ndi_command_VER_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    struct VER : command_base<VER>
    {
      enum reply_option
      {
        SYSTEM_CONTROL_PROCESSOR = 0,
        SYSTEM_CONTROL_UNIT_PROCESSOR = 3,
        SYSTEM_CONTROL_PROCESSOR_WITH_ENHANCED_REVISION_NUMBER,
        COMBINED_FIRMWARE_REVISION_NUMBER
      };

      VER(reply_option arg = SYSTEM_CONTROL_PROCESSOR) : option(arg) {}
      reply_option option;
    };

    namespace traits
    {
      template<>
      struct tag<VER>
      {
        typedef ascii_reply_tag reply_format;
        typedef version_information result_type;
      };

      template<>
      struct send<VER>
      {
        static inline void exec(serial_port* serial_port, VER& com)
        {
          ndi::send(serial_port, "VER:%d", com.option);
        }
      };

      template<>
      struct read<VER>
      {
        typedef tag<VER>::result_type result_type;
        static inline result_type exec(std::deque<char>& rep, VER& com)
        {
          version_information version;

          std::string version_strings(rep.begin(), rep.end());
          std::istringstream iss(version_strings);

          if (com.option == VER::COMBINED_FIRMWARE_REVISION_NUMBER)
          {
            if (!std::getline(iss, version.combined_firmware_revision))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
          }
          else
          {
            if (!std::getline(iss, version.type_of_firmware))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
            if (!std::getline(iss, version.ndi_serial_number))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
            if (com.option == VER::SYSTEM_CONTROL_PROCESSOR || com.option == VER::SYSTEM_CONTROL_PROCESSOR_WITH_ENHANCED_REVISION_NUMBER)
            {
              if (!std::getline(iss, version.characterization_date))
              {
                throw std::runtime_error("invalid_reply");
                //throw tracking_exception(error_code::invalid_reply);
              }
            }
            if (!std::getline(iss, version.freeze_tag))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
            if (!std::getline(iss, version.freeze_date))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
            if (!std::getline(iss, version.copyright_information))
            {
              throw std::runtime_error("invalid_reply");
              //throw tracking_exception(error_code::invalid_reply);
            }
          }
          if (static_cast<size_t>(iss.tellg()) != version_strings.size())
          {
            //ML_LOG_WARNING(logger::get()) << "too many lines in ver rep";
          }

          return version;
        }
      };
    }
  }
}

#endif
