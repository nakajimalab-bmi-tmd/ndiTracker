#include "ndi_command_handler.hpp"
#include "COMM.hpp"


namespace device {
  namespace ndi {

    DWORD BaudRate(baud_rate ndi_baudrate)
    {
      static const std::map<baud_rate, DWORD> baud_map = {
        { baud_rate::COMM_9600, CBR_9600 },
        { baud_rate::COMM_38400, CBR_38400 },
        { baud_rate::COMM_57600, CBR_57600 },
        { baud_rate::COMM_115200, CBR_115200 },
        { baud_rate::COMM_921600, 921600 },
        { baud_rate::COMM_1228739, CBR_19200 }
      };

      return baud_map.at(ndi_baudrate);
    }


  }
}
