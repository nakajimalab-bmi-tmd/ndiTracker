#include "python_polaris.h"
#include <boost/python/numpy.hpp>

void pyPolaris::auto_connect()
{
	impl.auto_connect();
}

void pyPolaris::connect(std::string const& port_name)
{
	impl.connect(port_name);
}

void pyPolaris::reset_system()
{
	impl.reset_system();
}

uint16_t pyPolaris::add_passive_tracker(std::string const& rom_file)
{
	return impl.add_passive_tracker(rom_file);
}

std::vector<uint16_t> pyPolaris::configure_active_tracker()
{
	return impl.configure_active_trackers();
}

void pyPolaris::start()
{
	impl.start();
}

void pyPolaris::stop()
{
	impl.stop();
}

void pyPolaris::update()
{
	impl.update();
}

boost::python::tuple pyPolaris::get(uint16_t port)
{
	namespace python = boost::python;
	namespace numpy = python::numpy;

	auto const& tool = impl.get_tracker_tool(port);

	bool is_valid = tool.get_handle_data().status.is_valid();
	numpy::ndarray translation = numpy::zeros(python::make_tuple(3), numpy::dtype::get_builtin<float>());
	numpy::ndarray quaternion = numpy::zeros(python::make_tuple(4), numpy::dtype::get_builtin<float>());
	translation[0] = tool.get_handle_data().transform_data.translation[0];
	translation[1] = tool.get_handle_data().transform_data.translation[1];
	translation[2] = tool.get_handle_data().transform_data.translation[2];
	quaternion[0] = tool.get_handle_data().transform_data.w;
	quaternion[1] = tool.get_handle_data().transform_data.qx;
	quaternion[2] = tool.get_handle_data().transform_data.qy;
	quaternion[3] = tool.get_handle_data().transform_data.qz;
	return python::make_tuple(is_valid, translation, quaternion);
}
