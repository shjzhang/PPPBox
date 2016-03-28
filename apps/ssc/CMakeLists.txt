# apps/filetools/CMakeLists.txt

add_executable(ssc2msc SSC2MSC.cpp)
target_link_libraries(ssc2msc pppbox)
install (TARGETS ssc2msc DESTINATION bin)

