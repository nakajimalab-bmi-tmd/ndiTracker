cmake_minimum_required (VERSION 3.8)

add_executable(polaris_test polaris_test.cpp)
target_link_libraries(polaris_test PRIVATE ndiTracker)

configure_file(8700340.rom ${CMAKE_CURRENT_BINARY_DIR}/8700340.rom COPYONLY)
configure_file(romfiles.txt ${CMAKE_CURRENT_BINARY_DIR}/romfiles.txt COPYONLY)

install(TARGETS polaris_test 
	RUNTIME DESTINATION bin
)
install(FILES 8700340.rom DESTINATION bin)
install(FILES romfiles.txt DESTINATION bin)
