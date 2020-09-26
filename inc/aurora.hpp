#ifndef include_aurora_hpp
#define include_aurora_hpp

#if defined(ndiTracker_SRC)
# define NDI_TRACKER_EXT __declspec(dllexport)
#else
# define NDI_TRACKER_EXT __declspec(dllimport)
#endif

#include "ndi_tracker.hpp"

namespace device {

  namespace ndi {

   
    class aurora;

    template<>
    struct COMM<aurora> : command_base <COMM<aurora>>
    {
      static comm_param const default;
      comm_param val;

      COMM(comm_param param = default) : val(param) {}
    };

    class aurora_sensor : public tracker_tool<aurora>
    {
      friend class aurora;
    public:
    };

    template <>
    struct tracker_tool_tag<aurora>
    {
      using tool_type = aurora_sensor;
    };

    class aurora : public tracker<aurora>
    {
    public:
      static std::string const& usb_port_name();
      std::string const& get_device_name() const;
      aurora();
      ~aurora();
      std::vector<uint16_t> configure_sensors();
      void update();

    };


  }
}
#endif
