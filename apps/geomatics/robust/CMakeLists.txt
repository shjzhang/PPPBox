# apps/geomatics/robust/CMakeLists.txt

add_executable(lsfilt lsfilt.cpp)
target_link_libraries(lsfilt pppbox)
install (TARGETS lsfilt DESTINATION bin)

add_executable(rstats rstats.cpp)
target_link_libraries(rstats pppbox)
install (TARGETS rstats DESTINATION bin)

