
#include "ndi_tracker.hpp"

namespace device
{
	namespace ndi
	{
		class polaris;

		class polaris_tracker : public tracker_tool<polaris>
		{
			friend polaris;
		};

		template<>
		struct COMM<polaris> : command_base <COMM<polaris>>
		{
			static comm_param const default;
			comm_param val;
			COMM(comm_param param = default) : val(param) {}
		};

		template<>
		struct tracker_tool_tag<polaris>
		{
			using tool_type = polaris_tracker;
		};

		class polaris : public tracker<polaris>
		{
		public:
			static std::string const& usb_port_name();
			std::string const& get_device_name() const;

			polaris();
			~polaris();

			std::vector<uint16_t> configure_active_trackers();
			uint16_t add_passive_tracker(std::string const& rom_file);

			void update();
		};

	}
}

