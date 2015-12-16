# apps/geomatics/cycleslips/CMakeLists.txt

add_executable(DiscFix DiscFix.cpp)
target_link_libraries(DiscFix pppbox)
install (TARGETS DiscFix DESTINATION bin)

