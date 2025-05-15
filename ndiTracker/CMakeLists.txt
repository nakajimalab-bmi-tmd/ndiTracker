# CMakeList.txt : CMake project for ndiTracker, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
find_package(boost_asio CONFIG REQUIRED)
find_package(boost_crc CONFIG REQUIRED)
find_package(boost_dynamic_bitset CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

set(ndiTracker_SRCS
	src/serial_port.cpp
	src/ndi_exception.cpp
	src/ndi_command_io.cpp
	src/COMM.cpp
	src/BX.cpp
	src/TX.cpp
	src/aurora.cpp
	src/polaris.cpp
)

set(ndiTracker_HEADS
	inc/aurora.hpp
	inc/binary_converter.hpp
	inc/BX.hpp
	inc/comm.hpp
	inc/INIT.hpp
	inc/ndi_command_base.hpp
	inc/ndi_command_handler.hpp
	inc/ndi_command_io.hpp
	inc/ndi_command_tag.hpp
	inc/ndi_exception.hpp
	inc/ndi_tracker.hpp
	inc/ndi_tracker_tool.hpp
	inc/ndi_types.hpp
	inc/PENA.hpp
	inc/PHINF.hpp
	inc/PHRQ.hpp
	inc/PHSR.hpp
	inc/PINIT.hpp
	inc/polaris.h
	inc/PVWR.hpp
	inc/RESET.hpp
	inc/serial_port.hpp
	inc/TSTART.hpp
	inc/TSTOP.hpp
	inc/TX.hpp
	inc/VER.hpp
)

add_library(ndiTracker STATIC ${ndiTracker_SRCS})
target_sources(ndiTracker PRIVATE
	src/aurora.cpp
	src/BX.cpp
	src/COMM.cpp
	src/ndi_command_io.cpp
	src/ndi_exception.cpp
	src/polaris.cpp
	src/serial_port.cpp
	src/TX.cpp
)

target_include_directories(ndiTracker PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

set_target_properties(ndiTracker PROPERTIES	
	PUBLIC_HEADER "${ndiTracker_HEADS}"
	)

target_link_libraries(ndiTracker PRIVATE Boost::asio Boost::crc Boost::dynamic_bitset Eigen3::Eigen)

install(TARGETS ndiTracker
	EXPORT ndiTracker
	LIBRARY			DESTINATION		lib
	INCLUDES		DESTINATION		include
	PUBLIC_HEADER	DESTINATION		include/ndiTracker
)

install(EXPORT ndiTracker
    DESTINATION share/cmake/ndiTracker/
    EXPORT_LINK_INTERFACE_LIBRARIES
)
