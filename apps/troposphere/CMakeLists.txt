# apps/tropodelay/CMakeLists.txt

add_executable(calztd calztd.cpp)
target_link_libraries(calztd pppbox)
install (TARGETS calztd DESTINATION bin)
add_test (calztd Test calztd)
