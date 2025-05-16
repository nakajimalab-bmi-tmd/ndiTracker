# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("ndiTracker")
add_subdirectory(ndiTracker)

option(BUILD_TEST "Build test executables" OFF)
if (BUILD_TEST)
	add_subdirectory(test/aurora)
	add_subdirectory(test/polaris)
endif()

