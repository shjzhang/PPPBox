# apps/differential/CMakeLists.txt


add_executable(vecsol vecsol.cpp)
target_link_libraries(vecsol pppbox)
install (TARGETS vecsol DESTINATION bin)
