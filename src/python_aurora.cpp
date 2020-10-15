#include "python_aurora.h"
#include <boost/python/numpy.hpp>

void pyAurora::auto_connect()
{
	impl.auto_connect();
}

void pyAurora::connect(std::string const& port_name)
{
	impl.connect(port_name);
}

void pyAurora::reset_system()
{
	impl.reset_system();
}

std::vector<uint16_t> pyAurora::configure_sensors()
{
	return impl.configure_sensors();
}

void pyAurora::start()
{
	impl.start();
}

void pyAurora::stop()
{
	impl.stop();
}

void pyAurora::update()
{
	impl.update();
}

boost::python::tuple pyAurora::get(uint16_t port)
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
