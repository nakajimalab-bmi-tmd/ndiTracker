#include <sdkddkver.h>

#include "serial_port.hpp"
#include <SetupAPI.h>
#include <boost/system/windows_error.hpp>

#pragma comment(lib, "SetupAPI.lib")

namespace device {


  std::vector<port_description> serial_port::get_active_ports()
  {
    std::vector<port_description> ports;

    GUID guid;
    DWORD size;
    auto ret = SetupDiClassGuidsFromNameA("PORTS", &guid, 1, &size);
    if (!ret) throw boost::system::system_error(boost::system::windows_error::windows_error_code(::GetLastError()));
    auto info_set = ::SetupDiGetClassDevsA(&guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_PROFILE);

    if (info_set)
    {
      SP_DEVINFO_DATA info_data;
      info_data.cbSize = sizeof(SP_DEVINFO_DATA);

      DWORD idx = 0;
      while (SetupDiEnumDeviceInfo(info_set, idx++, &info_data))
      {
        DWORD prop, size;
        std::vector<char> buffer(MAX_PATH, 0);
        SetupDiGetDeviceRegistryPropertyA(info_set, &info_data, SPDRP_FRIENDLYNAME, &prop, (LPBYTE)buffer.data(), (DWORD)buffer.size(), &size);
        std::string friendly_name(buffer.data());

        buffer.clear();
        buffer.resize(MAX_PATH);
        SetupDiGetDeviceRegistryPropertyA(info_set, &info_data, SPDRP_HARDWAREID, &prop, (LPBYTE)buffer.data(), (DWORD)buffer.size(), &size);
        std::string hardware_id(buffer.data());

        buffer.clear();
        buffer.resize(MAX_PATH);
        SetupDiGetDeviceRegistryPropertyA(info_set, &info_data, SPDRP_DEVICEDESC, &prop, (LPBYTE)buffer.data(), (DWORD)buffer.size(), &size);
        std::string device_desc(buffer.data());

        if (auto key = SetupDiOpenDevRegKey(info_set, &info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ))
        {
          buffer.clear();
          buffer.resize(MAX_PATH, 0);
          DWORD size = MAX_PATH;
          DWORD type = REG_SZ;
          RegQueryValueExA(key, "PortName", 0, &type, (LPBYTE)buffer.data(), &size);
          RegCloseKey(key);

          std::string port_name(buffer.data());

          if (port_name.compare(0, 3, "COM") == 0)
          {
            port_description desc = { port_name, friendly_name, hardware_id, device_desc };
            ports.push_back(desc);
          }
        }
      }
    }

    return ports;
  }

  std::vector<port_description> const serial_port::find_port_from_hardware_id(std::string const& hardware_id)
  {
    std::vector<port_description> target_list;
    auto const list = serial_port::get_active_ports();

    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if (it->hardware_id.size() < hardware_id.size()) continue;
      if (it->hardware_id.compare(0, hardware_id.size(), hardware_id) == 0)
      {
        target_list.push_back(*it);
      }
    }
    return target_list;
  }

  std::string const serial_port::find_port_from_friendly_name(std::string const& keyword)
  {
    auto const list = serial_port::get_active_ports();

    auto it = std::find_if(list.begin(), list.end(), [keyword](port_description const& port_desc)
    {
      return port_desc.friendly_name.find(keyword) != std::string::npos;
    });

    if (it == list.end()) throw std::runtime_error("not found");
    return it->com_port;
  }

  serial_port::serial_port(boost::asio::io_context& io_context) : io_context_{ io_context }, serial_port_(io_context), timer(io_context), timeout(0) {}
  void serial_port::open(std::string const& device)
  {
    this->close();
    serial_port_.open(device);
    serial_port_.set_option(baud_rate(9600));
    serial_port_.set_option(character_size(8));
    serial_port_.set_option(parity(boost::asio::serial_port::parity::none));
    serial_port_.set_option(stop_bits(boost::asio::serial_port::stop_bits::one));
    serial_port_.set_option(flow_control(flow_control::none));
    this->flush();
  }
  void serial_port::close()
  {
    if (serial_port_.is_open()) serial_port_.close();
  }

  void serial_port::set_baud_rate(unsigned int const baud_rate_value)
  {
    serial_port_.set_option(baud_rate(baud_rate_value));
  }

  void serial_port::set_flow_control(flow_control::type const flow_control_type)
  {
    serial_port_.set_option(flow_control(flow_control_type));
  }

  void serial_port::send_break()
  {
    if (::SetCommBreak(serial_port_.native_handle()) == 0)
    {
      ::abort();
    }
    ::Sleep(1000);
    ::ClearCommBreak(serial_port_.native_handle());
    this->flush();
  }

  std::tuple<char const*, size_t> serial_port::read_until_impl(size_t const& maximum_size, char const terminate)
  {
    boost::asio::streambuf b(maximum_size);
    size_t const length = boost::asio::read_until(serial_port_, b, terminate);
    return std::make_tuple(boost::asio::buffer_cast<char const *>(b.data()), length);
  }

  void serial_port::write(std::string const& command)
  {
    boost::asio::write(serial_port_, boost::asio::buffer(command));
  }

  void serial_port::flush()
  {
    ::PurgeComm(serial_port_.native_handle(), PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
  }

  void serial_port::set_timeout(uint16_t const millisecond)
  {
    this->timeout = std::chrono::milliseconds(millisecond);
  }


  void serial_port::on_timeout(boost::system::error_code const& error)
  {
    if (!error)
    {
      serial_port_.cancel();
    }
  }
}
