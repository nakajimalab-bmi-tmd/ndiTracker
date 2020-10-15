#include <polaris.h>
#include <boost/python/tuple.hpp>

class pyPolaris
{
public:
	void auto_connect();
	void connect(std::string const& port_name);
	void reset_system();
	uint16_t add_passive_tracker(std::string const& rom_file);
	std::vector<uint16_t> configure_active_tracker();
	void start();
	void stop();
	void update();
	boost::python::tuple get(uint16_t port);

private:
	device::ndi::polaris impl;
};
