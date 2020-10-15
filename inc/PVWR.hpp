#pragma once
#ifndef DEVICE_NDI_PVWR_HPP
#define DEVICE_NDI_PVWR_HPP
#include <fstream>
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"
#include "serial_port.hpp"

namespace device {
	namespace ndi {

		struct PVWR : command_base<PVWR>
		{
			constexpr static size_t SROM_FILE_SIZE = 1024;

			uint16_t port_handle_;
			uint16_t start_address_ = 0;
			std::array<uint8_t, SROM_FILE_SIZE> tool_def_data_;

			//PVWR(uint16_t port_handle, std::array<uint8_t, SROM_FILE_SIZE>& tool_def_data) : port_handle_(port_handle), tool_def_data_(tool_def_data) {}
			PVWR(uint16_t port_handle, const char* filename);
		};

		PVWR::PVWR(uint16_t port_handle, const char* filename) : port_handle_(port_handle)
		{
			std::ifstream ifs(filename, std::ios::binary);
			if (!ifs) throw std::runtime_error("file not found");
			tool_def_data_.fill(0);
			ifs.read(reinterpret_cast<char*>(tool_def_data_.data()), SROM_FILE_SIZE);
			ifs.close();
		}

		namespace traits
		{
			template<>
			struct tag<PVWR>
			{
				typedef ascii_reply_tag reply_format;
				typedef void result_type;
			};

			template<>
			struct send<PVWR>
			{
				static inline void exec(serial_port* serial_port, PVWR& com)
				{
					static const size_t size = 129;
					std::array<char, size> hex_data;
					hex_data.fill(0);
					for (size_t i = 0; i < 64; ++i)
					{
						sprintf_s(hex_data.data() + 2 * i, 3, "%02X", com.tool_def_data_[com.start_address_ + i]);
					}
					ndi::send(serial_port, "PVWR:%02X%04X%.128s", com.port_handle_, com.start_address_, hex_data.data());
				}
			};
		}

		template<>
		inline traits::tag<PVWR>::result_type command::operator()<PVWR>(command_base<PVWR>& com) const
		{
			std::unique_lock<std::mutex> lock{ mutex };
			PVWR& pvwr = static_cast<PVWR&>(com);
			pvwr.start_address_ = 0;
			for (; pvwr.start_address_ < PVWR::SROM_FILE_SIZE; pvwr.start_address_ += 64)
			{
				send(serial_port, pvwr);
				read(recv<PVWR>(serial_port), pvwr);
			}
		}
	}
}


#endif
