#include <iostream>
#include <polaris.h>
#include <PHRQ.hpp>
#include <PVWR.hpp>

namespace device
{
	namespace ndi
	{
		comm_param const COMM<polaris>::default = { baud_rate::COMM_1228739, data_type::COMM_8N1, handshake::COMM_HANDSHAKE };

		std::string const& polaris::usb_port_name()
		{
			static const std::string name("NDI Polaris Spectra SCU Port");
			return name;
		}

		std::string const& polaris::get_device_name() const
		{
			static const std::string name("Polaris");
			return name;
		}

		polaris::polaris()
		{
		}

		polaris::~polaris()
		{
		}

		std::vector<uint16_t> polaris::configure_active_trackers()
		{
			std::vector<uint16_t> handles;
			auto ports = this->handler(ndi::PHSR());
			for (auto const& handle : ports)
			{
				if (this->tracker_tools.count(handle.first) == 0)
				{
					this->tracker_tools[handle.first] = polaris_tracker{};
				}
				this->tracker_tools[handle.first].tool_data.transform_data.status = handle.second;
				if (!handle.second.is_<port_status::INITIALIZED>())
				{
					this->handler(PINIT(handle.first));
				}

				PHINF const& phinf = this->handler(PHINF(handle.first));
				if (auto const& tool_info = phinf.get_tool_information())
				{
					this->tracker_tools[handle.first].handle_information_.main_type = tool_info->get_tool_type().name();
					this->tracker_tools[handle.first].handle_information_.manufacture_id = tool_info->get_manufacture_id();
					this->tracker_tools[handle.first].handle_information_.tool_revision = tool_info->get_tool_revision();

					auto tool_type = tool_info->get_tool_type().get();
					using tool_type_ = PHINF::tool_information::tool_type;
					if (tool_type != tool_type_::main_type::Strober_Tool_Docking_Station)
					{
						this->handler(PENA(handle.first));
						handles.push_back(handle.first);
					}
				}
				if (auto const& part_number = phinf.get_tool_part_number())
				{
					this->tracker_tools[handle.first].handle_information_.part_name = part_number->get();
				}
			}
			return handles;
		}

		uint16_t polaris::add_passive_tracker(std::string const& rom_file)
		{
			auto port_handle = this->handler(PHRQ{});
			this->handler(PVWR{ port_handle, rom_file.c_str() });

			return uint16_t();
		}

		void polaris::update()
		{
			this->update_tracker_tools();
		}

	}
}

