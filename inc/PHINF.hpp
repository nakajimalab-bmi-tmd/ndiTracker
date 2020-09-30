#ifndef include_ndi_command_PHINF_hpp
#define include_ndi_command_PHINF_hpp
#include <bitset>
#include <boost/optional.hpp>
#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

#define ENUM_ASSIGN(Type) \
  case Type: \
    this->main_type = Type; \
    this->type_name = L ## #Type;\
    break;


namespace device {
	namespace ndi {

		struct PHINF : command_base<PHINF>
		{
			enum reply_option_type
			{
				Tool_Information = 0x0001,
				Tool_Part_Number = 0x0004,
				//Switch_Visible_LED_Information = 0x0008,
				//Physical_Port_Location = 0x0020,
				//GPIO_Line_Definition = 0x0040,
				//Sensor_Configuration_Physical_Port_Location = 0x0080
			};

			struct tool_information
			{
			public:
				tool_information(std::queue<char>& q)
				{
					tool_type_ = tool_type(q);

					for (int i = 0; i < 12; ++i)
					{
						manufacture_id.push_back(q.front());
						q.pop();
					}

					for (int i = 0; i < 3; ++i)
					{
						tool_revision.push_back(q.front());
						q.pop();
					}

					serial_number = hex_converter<uint32_t>(q, 8);

					port_handle_status.set(hex_converter<uint8_t>(q, 2));

				}
				static constexpr const size_t length = 33;

				struct tool_type
				{
				public:
					enum main_type
					{
						Reference = 0x01,
						Probe = 0x02,
						Button = 0x03,
						Software_Defined = 0x04,
						Microscope_Tracker = 0x05,
						Calibration_Device = 0x07,
						Strober_Tool_Docking_Station = 0x08,
						Isolation_Box = 0x09,
						C_Arm_Tracker = 0x0A,
						Catheter = 0x0B,
						GPIO_Device = 0x0C
					};
					tool_type() {}
					tool_type(std::queue<char>& q)
					{
						this->set(static_cast<uint8_t>(hex_converter<uint16_t>(q, 2)));
						this->num_switches = hex_converter<uint16_t>(q, 1);
						this->num_LEDs = hex_converter<uint16_t>(q, 1);
						q.pop();
						q.pop();
						this->subtype = hex_converter<uint16_t>(q, 2);
					}

					std::wstring const& name() const
					{
						return this->type_name;
					}

					main_type get() const
					{
						return this->main_type;
					}

					uint16_t get_num_switches() const
					{
						return this->num_switches;
					}

					uint16_t get_num_LEDs() const
					{
						return this->num_LEDs;
					}

					uint16_t get_subtype() const
					{
						return this->subtype;
					}

				private:
					void set(uint8_t arg)
					{
						switch (arg)
						{
							ENUM_ASSIGN(Reference);
							ENUM_ASSIGN(Probe);
							ENUM_ASSIGN(Button);
							ENUM_ASSIGN(Software_Defined);
							ENUM_ASSIGN(Microscope_Tracker);
							ENUM_ASSIGN(Calibration_Device);
							ENUM_ASSIGN(Strober_Tool_Docking_Station);
							ENUM_ASSIGN(Isolation_Box);
							ENUM_ASSIGN(C_Arm_Tracker);
							ENUM_ASSIGN(Catheter);
							ENUM_ASSIGN(GPIO_Device);
						default:
							throw std::runtime_error("Wrong ToolInformation.ToolType");
						}
					}

					main_type main_type;
					uint16_t num_switches;
					uint16_t num_LEDs;
					uint16_t subtype;

					std::wstring type_name;
				};

				tool_type const& get_tool_type() const
				{
					return tool_type_;
				}

				std::wstring const& get_manufacture_id() const
				{
					return manufacture_id;
				}

				std::wstring const& get_tool_revision() const
				{
					return tool_revision;
				}

			private:

				tool_type tool_type_;

				std::wstring manufacture_id;

				std::wstring tool_revision;

				uint32_t serial_number;

				struct port_handle_status_def
				{
				public:
					port_handle_status_def() {}
					void set(uint8_t arg)
					{
						data = std::bitset<8>(arg);
					}

					bool occupied() const { return data[0]; }
					bool GPIO1_closed() const { return data[1]; }
					bool GPIO2_closed() const { return data[2]; }
					bool GPIO3_closed() const { return data[3]; }
					bool port_handle_initialized() const { return data[4]; }
					bool port_handle_enabled() const { return data[5]; }
				private:
					std::bitset<8> data;
				} port_handle_status;
			};

			struct tool_part_number
			{
			public:
				static constexpr const size_t length = 20;

				tool_part_number(std::queue<char>& q)
				{
					data.clear();
					for (int i = 0; i < 20; ++i)
					{
						data.push_back(q.front());
						q.pop();
					}
				}

				std::wstring const& get() const
				{
					return data;
				}

			private:
				std::wstring data;
			};

			struct switch_visible_LED_information
			{
				switch_visible_LED_information(std::queue<char>& reply)
				{

				}

				bool input_supported_GPIO1() const
				{
					return data[1];
				}

				bool input_supported_GPIO2() const
				{
					return data[2];
				}

				bool input_supported_GPIO3() const
				{
					return data[3];
				}

				bool tool_tracking_LED_supported() const
				{
					return data[4];
				}

				bool visible_LED_supported_GPIO1() const
				{
					return data[5];
				}

				bool visible_LED_supported_GPIO2() const
				{
					return data[6];
				}

				bool visible_LED_supported_GPIO3() const
				{
					return data[7];
				}

				std::bitset<8> data;
			};

			explicit PHINF(uint16_t port_handle, uint8_t opt = Tool_Information | Tool_Part_Number) : port_handle_(port_handle), option(opt) {}

			uint16_t const port_handle() const { return port_handle_; }
			uint8_t const reply_option() const { return option; }

			void set(tool_information arg) { this->tool_information_ = arg; }
			void set(tool_part_number arg) { this->tool_part_number_ = arg; }

			boost::optional<tool_information> const get_tool_information() const { return tool_information_; }
			boost::optional<tool_part_number> const get_tool_part_number() const { return tool_part_number_; }


		private:
			uint16_t port_handle_;
			uint8_t option;

			boost::optional<tool_information> tool_information_;
			boost::optional<tool_part_number> tool_part_number_;
		};

		namespace traits
		{
			template<>
			struct tag < PHINF >
			{
				typedef ascii_reply_tag reply_format;
				typedef PHINF result_type;
			};

			template<>
			struct timeout<PHINF>
			{
				static constexpr uint32_t value()
				{
					return 5000;
				}
			};

			template<>
			struct send < PHINF >
			{
				static inline void exec(serial_port* serial_port, PHINF& com)
				{
					ndi::send(serial_port, "PHINF:%02X%04X", com.port_handle(), com.reply_option());
				}
			};

			template<>
			struct read<PHINF>
			{
				typedef tag<PHINF>::result_type result_type;

				static inline result_type exec(std::deque<char>& rep, PHINF& com)
				{
					std::queue<char> queue(std::move(rep));

					if (com.reply_option() & PHINF::Tool_Information)
					{
						if (queue.size() < PHINF::tool_information::length) throw std::runtime_error("Invalid reply");
						com.set(PHINF::tool_information(queue));
					}

					if (com.reply_option() & PHINF::Tool_Part_Number)
					{
						if (queue.size() < PHINF::tool_part_number::length) throw std::runtime_error("Invalid reply");
						com.set(PHINF::tool_part_number(queue));
					}
					return com;
				}
			};
		}

	}
}

#undef ENUM_ASSIGN

#endif
#pragma once
