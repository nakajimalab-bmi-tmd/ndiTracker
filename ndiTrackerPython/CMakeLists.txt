cmake_minimum_required (VERSION 3.8)
find_package(Python3 COMPONENTS Development REQUIRED)
message("Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message("Python3_LIBRARIES: ${Python3_LIBRARIES}")
find_package(Boost COMPONENTS python38 numpy38 system REQUIRED)

set(ndiTracker_PYTHON_SRC
	python_polaris.cpp
	python_aurora.cpp
	python_export.cpp
)
add_library(ndiTrackerPython SHARED ${ndiTracker_PYTHON_SRC})
set_target_properties(ndiTrackerPython PROPERTIES 
	OUTPUT_NAME "ndiTracker"
	SUFFIX ".pyd")
target_include_directories(ndiTrackerPython PRIVATE ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})
target_link_libraries(ndiTrackerPython PRIVATE ndiTracker ${Python3_LIBRARIES} ${Boost_LIBRARIES})

configure_file(aurora_test.py ${CMAKE_CURRENT_BINARY_DIR}/aurora_test.py COPYONLY)
configure_file(polaris_test.py ${CMAKE_CURRENT_BINARY_DIR}/aurora_test.py COPYONLY)


file(GLOB DLLs "${CMAKE_CURRENT_BINARY_DIR}/*.dll")

install(TARGETS ndiTrackerPython
	RUNTIME DESTINATION Python
	LIBRARY DESTINATION Python
)
install(FILES ${DLLs} DESTINATION Python)
install(FILES aurora_test.py DESTINATION Python)
install(FILES polaris_test.py DESTINATION Python)
