# apps/tropodelay/CMakeLists.txt

add_executable(calztd calztd.cpp)
target_link_libraries(calztd gpstk)
install (TARGETS calztd DESTINATION bin)
add_test (calztd Test calztd)
