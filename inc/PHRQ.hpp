#pragma once

#ifndef DEVICE_NDI_PHRQ_HPP
#define DEVICE_NDI_PHRQ_HPP

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
	namespace ndi {

		struct PHRQ : command_base<PHRQ>
		{
			enum tool_type
			{
				Wired = 0,
				Wireless = 1
			};

			PHRQ(tool_type type = Wireless) : tool(type) {}
			PHRQ(uint16_t port_number, tool_type type = Wired) : tool(type), port_num(port_number) {}

			tool_type tool;
			uint16_t port_num = 0;
		};

		namespace traits
		{
			template<>
			struct tag<PHRQ>
			{
				typedef ascii_reply_tag reply_format;
				typedef uint16_t result_type;
			};

			template<>
			struct send<PHRQ>
			{
				static inline void exec(serial_port* serial_port, PHRQ& com)
				{
					if (com.tool == PHRQ::Wireless)
					{
						ndi::send(serial_port, "PHRQ:*********1****");
					}
					else
					{
						ndi::send(serial_port, "PHRQ:**********%02d**", com.port_num);
					}
				}
			};
			template<>
			struct read<PHRQ>
			{
				typedef tag<PHRQ>::result_type result_type;
				static inline result_type exec(std::deque<char>& rep, PHRQ&)
				{
					if (rep.size() != 2) throw std::runtime_error{"invalid reply"};
					std::queue<char> queue{ std::move(rep) };
					return hex_converter<uint16_t>(queue, 2);
				}
			};
		}
	}
}

#endif
