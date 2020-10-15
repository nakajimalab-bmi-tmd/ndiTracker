#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "python_polaris.h"
#include "python_aurora.h"

BOOST_PYTHON_MODULE(ndiTracker)
{
	using namespace boost::python;

	Py_Initialize();
	numpy::initialize();

	class_<pyAurora, boost::noncopyable>("aurora")
		.def("configure_sensors", &pyAurora::configure_sensors)
		.def("auto_connect", &pyAurora::auto_connect)
		.def("connect", &pyAurora::connect)
		.def("reset_system", &pyAurora::reset_system)
		.def("start", &pyAurora::start)
		.def("stop", &pyAurora::stop)
		.def("update", &pyAurora::update)
		.def("get", &pyAurora::get);

	class_<pyPolaris, boost::noncopyable>("polaris")
		.def("add_passive_tracker", &pyPolaris::add_passive_tracker)
		.def("configure_active_tracker", &pyPolaris::configure_active_tracker)
		.def("auto_connect", &pyPolaris::auto_connect)
		.def("connect", &pyPolaris::connect)
		.def("reset_system", &pyPolaris::reset_system)
		.def("start", &pyPolaris::start)
		.def("stop", &pyPolaris::stop)
		.def("update", &pyPolaris::update)
		.def("get", &pyPolaris::get);

	class_<std::vector<uint16_t> >("vectorU16")
		.def(vector_indexing_suite<std::vector<uint16_t> >());
}
