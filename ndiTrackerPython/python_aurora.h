#include <aurora.hpp>
#include <boost/python/tuple.hpp>

class pyAurora
{
public:
	void auto_connect();
	void connect(std::string const& port_name);
	void reset_system();
	std::vector<uint16_t> configure_sensors();
	void start();
	void stop();
	void update();
	boost::python::tuple get(uint16_t port);

private:
	device::ndi::aurora impl;
};
