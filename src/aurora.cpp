#include "aurora.hpp"

namespace device {

	namespace ndi {

		aurora::aurora()
		{
		}

		aurora::~aurora() {}

		std::string const& aurora::usb_port_name()
		{
			static const std::string name("NDI Aurora SCU Port");
			return name;
		}

		std::string const& aurora::get_device_name() const
		{
			static const std::string name("Aurora");
			return name;
		}

		std::vector<uint16_t> aurora::configure_sensors()
		{
			std::vector<uint16_t> handles;
			auto ports = this->handler(ndi::PHSR());
			for (auto const& handle : ports)
			{
				if (this->tracker_tools.count(handle.first) == 0)
				{
					this->tracker_tools[handle.first] = aurora_sensor{};
				}
				this->tracker_tools[handle.first].tool_data.transform_data.status = handle.second;
				if (!handle.second.is_<port_status::INITIALIZED>())
				{
					this->handler(PINIT(handle.first));
					this->handler(PENA(handle.first));
				}

				PHINF const& phinf = this->handler(PHINF(handle.first));
				if (auto const& tool_info = phinf.get_tool_information())
				{
					this->tracker_tools[handle.first].handle_information_.main_type = tool_info->get_tool_type().name();
					this->tracker_tools[handle.first].handle_information_.manufacture_id = tool_info->get_manufacture_id();
					this->tracker_tools[handle.first].handle_information_.tool_revision = tool_info->get_tool_revision();
				}

				if (auto const& part_number = phinf.get_tool_part_number())
				{
					this->tracker_tools[handle.first].handle_information_.part_name = part_number->get();
				}
				handles.push_back(handle.first);
			}
			return handles;
		}

		void aurora::update()
		{
			this->update_tracker_tools();
		}

		comm_param const COMM<aurora>::default = { baud_rate::COMM_921600, data_type::COMM_8N1, handshake::COMM_HANDSHAKE };

	}
}
