# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ndiTracker")
find_package(Python3 COMPONENTS Development REQUIRED)
message("Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message("Python3_LIBRARIES: ${Python3_LIBRARIES}")
find_package(Boost COMPONENTS python38 numpy38 system REQUIRED)
# Include sub-projects.


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

set(ndiTracker_inc
	inc/
)

set(ndiTracker_PYTHON_SRC
	src/python_polaris.cpp
	src/python_aurora.cpp
	src/python_export.cpp
)

add_library(ndiTracker SHARED ${ndiTracker_SRCS} ${ndiTracker_PYTHON_SRC})
set_target_properties(ndiTracker PROPERTIES SUFFIX ".pyd")
target_include_directories(ndiTracker PUBLIC ${ndiTracker_inc} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})
target_compile_definitions(ndiTracker PRIVATE NDI_TRACKER_SRC)
target_link_libraries(ndiTracker PUBLIC ${Python3_LIBRARIES} ${Boost_LIBRARIES})

add_executable(aurora_test test/aurora_test.cpp ${ndiTracker_SRCS})
target_include_directories(aurora_test PUBLIC ${ndiTracker_inc} ${Boost_INCLUDE_DIRS})
target_link_libraries(aurora_test PUBLIC ${Boost_LIBRARIES})

add_executable(polaris_test test/polaris_test.cpp ${ndiTracker_SRCS})
target_include_directories(polaris_test PUBLIC ${ndiTracker_inc} ${Boost_INCLUDE_DIRS})
target_link_libraries(polaris_test PUBLIC ${Boost_LIBRARIES})

configure_file(test/aurora_test.py ${CMAKE_CURRENT_BINARY_DIR}/aurora_test.py COPYONLY)
configure_file(test/8700340.rom ${CMAKE_CURRENT_BINARY_DIR}/8700340.rom COPYONLY)
configure_file(test/romfiles.txt ${CMAKE_CURRENT_BINARY_DIR}/romfiles.txt COPYONLY)
