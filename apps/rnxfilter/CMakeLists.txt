# apps/performance/CMakeLists.txt

add_executable(rnxfilter rnxfilter.cpp)
target_link_libraries(rnxfilter pppbox)
install (TARGETS rnxfilter DESTINATION bin)


