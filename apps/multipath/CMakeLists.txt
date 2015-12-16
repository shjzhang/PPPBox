# apps/multipath/CMakeLists.txt


add_executable(mpsolve mpsolve.cpp ObsArray.cpp)
target_link_libraries(mpsolve pppbox)
install (TARGETS mpsolve DESTINATION bin)
