#include <aurora.hpp>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/tuple.hpp>

class aurora
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

void aurora::auto_connect()
{
	impl.auto_connect();
}

void aurora::connect(std::string const& port_name)
{
	impl.connect(port_name);
}

void aurora::reset_system()
{
	impl.reset_system();
}

std::vector<uint16_t> aurora::configure_sensors()
{
	return impl.configure_sensors();
}

void aurora::start()
{
	impl.start();
}

void aurora::stop()
{
	impl.stop();
}

void aurora::update()
{
	impl.update();
}

boost::python::tuple aurora::get(uint16_t port)
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

BOOST_PYTHON_MODULE(ndiTracker)
{
	using namespace boost::python;

	Py_Initialize();
	numpy::initialize();


	class_<aurora, boost::noncopyable>("aurora")
		.def("configure_sensors", &aurora::configure_sensors)
		.def("auto_connect", &aurora::auto_connect)
		.def("connect", &aurora::connect)
		.def("reset_system", &aurora::reset_system)
		.def("start", &aurora::start)
		.def("stop", &aurora::stop)
		.def("update", &aurora::update)
		.def("get", &aurora::get);

	class_<std::vector<uint16_t> >("vectorU16")
		.def(vector_indexing_suite<std::vector<uint16_t> >());
}
