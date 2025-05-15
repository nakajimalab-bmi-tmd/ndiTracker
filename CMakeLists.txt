# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ndiTracker")
add_subdirectory(ndiTracker)
add_subdirectory(test/aurora)
add_subdirectory(test/polaris)

option(ENABLE_PYTHON "Enable Python bindings" OFF)
if(ENABLE_PYTHON)
	add_subdirectory(ndiTrackerPython)
endif()
