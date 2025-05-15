cmake_minimum_required (VERSION 3.8)

find_package(boost_asio CONFIG REQUIRED)
find_package(boost_crc CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

add_executable(aurora_test aurora_test.cpp)
target_link_libraries(aurora_test PRIVATE ndiTracker Boost::asio Boost::crc cereal::cereal)

install(TARGETS aurora_test
	RUNTIME DESTINATION bin
)