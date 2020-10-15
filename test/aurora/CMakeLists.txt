cmake_minimum_required (VERSION 3.8)

add_executable(aurora_test aurora_test.cpp)
target_link_libraries(aurora_test PRIVATE ndiTracker)

install(TARGETS aurora_test
	RUNTIME DESTINATION bin
)