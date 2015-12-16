# apps/geomatics/JPLeph/CMakeLists.txt

add_executable(convertSSEph convertSSEph.cpp)
target_link_libraries(convertSSEph pppbox)
install (TARGETS convertSSEph DESTINATION bin)

add_executable(testSSEph testSSEph.cpp)
target_link_libraries(testSSEph pppbox)
install (TARGETS testSSEph DESTINATION bin)

