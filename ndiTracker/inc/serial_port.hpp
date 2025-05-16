#ifndef include_serial_port_hpp
#define include_serial_port_hpp

#include <future>
#include <sstream>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>

namespace device {

  struct port_description
  {
    std::string com_port;
    std::string friendly_name;
    std::string hardware_id;
    std::string device_desc;
  };

  class serial_port
  {
    typedef boost::asio::serial_port base;
    serial_port(serial_port const&) = delete;
    void operator=(serial_port const&) = delete;

  public:
    typedef base::baud_rate baud_rate;
    typedef base::character_size character_size;
    typedef base::parity parity;
    typedef base::stop_bits stop_bits;
    typedef base::flow_control flow_control;

    typedef std::tuple<character_size, parity, stop_bits> parameter_type;
    static const parameter_type SP_8N1;
    static const parameter_type SP_7E1;

    static std::vector<port_description> get_active_ports();
    static std::vector<port_description> const find_port_from_hardware_id(std::string const& hardware_id);
    static std::string const find_port_from_friendly_name(std::string const& friendly_name);

    serial_port(boost::asio::io_context& io_context);
    void open(std::string const& device);

    inline bool is_open() const
    {
      this->serial_port_.is_open();
    }

    void close();

    void set_baud_rate(unsigned int const baud_rate_value);
    void set_flow_control(flow_control::type const flow_control_type);

    void send_break();
    
    template<template<typename T, typename A = std::allocator<T>> class Container = std::vector>
    Container<char> read_until(char const terminate, size_t const maximum_size = (std::numeric_limits<size_t>::max)())
    {
      boost::system::error_code ec = boost::asio::error::would_block;
      size_t length = 0;
      boost::asio::streambuf b(maximum_size);
      boost::asio::async_read_until(serial_port_, b, terminate, 
        [&ec, &length](boost::system::error_code const& error_code, size_t bytes_transferred) { ec = error_code; length = bytes_transferred; }
      );

      if (this->timeout.count())
      {
        timer.expires_from_now(this->timeout);
        timer.async_wait(std::bind(&serial_port::on_timeout, this, std::placeholders::_1));
      }

      do
      {
          this->io_context_.run_one();
      } while (ec == boost::asio::error::would_block);

      if (ec == boost::asio::error::operation_aborted)
      {
        throw boost::system::system_error(ec);
      }
      
      timer.cancel();

      if (ec) throw boost::system::system_error(ec);

      char const* data = boost::asio::buffer_cast<char const*>(b.data());
      return Container<char>(data, data + length);
    }

    template<template<typename T, typename A = std::allocator<T>> class Container = std::vector>
    Container<char> read(size_t const maximum_size)
    {
      std::vector<char> d(maximum_size);
      boost::asio::read(this->serial_port_, boost::asio::buffer(d, maximum_size));
      return Container<char>(d.begin(), d.end());
    }

    void write(std::string const& command);
    void flush();

    void set_timeout(uint16_t const millisecond);

    inline boost::asio::serial_port& port()
    {
      return serial_port_;
    }

  private:
    void on_timeout(boost::system::error_code const& error);

    std::tuple<char const*, size_t> read_until_impl(size_t const& maximum_size, char const termination);
    boost::asio::io_context& io_context_;
    boost::asio::serial_port serial_port_;
    boost::asio::system_timer timer;
    std::chrono::milliseconds timeout;
  };
}

#endif
