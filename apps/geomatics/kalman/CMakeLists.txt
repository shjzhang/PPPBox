# apps/geomatics/kalman/CMakeLists.txt

add_executable(mergeSRI mergeSRI.cpp)
target_link_libraries(mergeSRI pppbox)
install (TARGETS mergeSRI DESTINATION bin)

add_executable(tkalm tkalm.cpp)
target_link_libraries(tkalm pppbox)
install (TARGETS tkalm DESTINATION bin)
