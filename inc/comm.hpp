#ifndef include_ndi_command_COMM_hpp
#define include_ndi_command_COMM_hpp

#include "serial_port.hpp"
#include "ndi_command_base.hpp"
#include "ndi_command_tag.hpp"
#include "ndi_command_io.hpp"

namespace device {
  namespace ndi {

    enum class baud_rate
    {
      COMM_9600 = 0,
      COMM_14400 = 1,
      COMM_19200 = 2,
      COMM_38400 = 3,
      COMM_57600 = 4,
      COMM_115200 = 5,
      COMM_921600 = 6,
      COMM_1228739 = 7,
    };

    enum class data_type
    {
      COMM_8N1 = 000,
      COMM_8N2 = 001,
      COMM_8O1 = 010,
      COMM_8O2 = 011,
      COMM_8E1 = 020,
      COMM_8E2 = 021,
      COMM_7N1 = 100,
      COMM_7N2 = 101,
      COMM_7O1 = 110,
      COMM_7O2 = 111,
      COMM_7E1 = 120,
      COMM_7E2 = 121,
    };

    enum class handshake
    {
      COMM_NO_HANDSHAKE = 0,
      COMM_HANDSHAKE = 1,
    };

    struct comm_param
    {
      baud_rate baud_rate;
      data_type data;
      handshake handshake;
    };

    DWORD BaudRate(baud_rate ndi_baudrate);

    template <typename Device> struct COMM : command_base <COMM<Device>> {};


    namespace traits
    {
      template<typename Device>
      struct tag < COMM<Device> >
      {
        typedef ascii_reply_tag reply_format;
        typedef has_postprocess_tag post_process;
        typedef void result_type;
      };

      template<typename Device>
      struct send < COMM<Device> >
      {
        static inline void exec(serial_port* serial_port, COMM<Device>& com)
        {
          ndi::send(serial_port, "COMM:%d%03d%d", com.val.baud_rate, com.val.data, com.val.handshake);
        }
      };
    }

    template<typename Device>
    struct post_command<typename COMM<Device>>// typename traits::tag<COMM<Device>>::result_type command<Device>::operator() < COMM<Device> > (command_base<COMM<Device>>& com) const
    {
      static void exec(device::serial_port* port, command_base<COMM<Device>>& com)
      {

        if (static_cast<COMM<Device>&>(com).val.data != data_type::COMM_8N1) throw ndi::exception(ndi::error_code::invalid_parameter);
        auto const& val = static_cast<COMM<Device>&>(com).val;
        uint32_t baudrate = BaudRate(val.baud_rate);
        auto flowctrl = val.handshake == handshake::COMM_NO_HANDSHAKE ? serial_port::flow_control::none : serial_port::flow_control::hardware;
        ::Sleep(100);

        //Update parameters
        port->set_baud_rate(baudrate);
        port->set_flow_control(flowctrl);
        return;
      }
    };
  }
}

#endif
