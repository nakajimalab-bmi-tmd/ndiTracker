#include <iostream>
#include "..\inc\polaris.h"


polaris::polaris()
{
	std::cout << "ctor" << std::endl;
}

polaris::~polaris()
{
	std::cout << "dtor" << std::endl;
}

void polaris::start()
{
	std::cout << "start" << std::endl;
}

void polaris::stop()
{
	std::cout << "stop" << std::endl;
}

void polaris::update()
{
	std::cout << "update" << std::endl;
}

//#include <boost/python.hpp>
//using namespace boost::python;
//
//BOOST_PYTHON_MODULE(ndiTracker)
//{
//	class_<polaris>("polaris")
//		.def("start", &polaris::start)
//		.def("stop", &polaris::stop)
//		.def("update", &polaris::update)
//		;
//}
