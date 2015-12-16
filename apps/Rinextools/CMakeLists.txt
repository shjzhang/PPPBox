# apps/Rinextools/CMakeLists.txt

add_executable(RinDump RinDump.cpp)
target_link_libraries(RinDump pppbox)
install (TARGETS RinDump DESTINATION bin)

add_executable(RinEdit RinEdit.cpp)
target_link_libraries(RinEdit pppbox)
install (TARGETS RinEdit DESTINATION bin)

add_executable(RinNav RinNav.cpp)
target_link_libraries(RinNav pppbox)
install (TARGETS RinNav DESTINATION bin)

add_executable(RinSum RinSum.cpp)
target_link_libraries(RinSum pppbox)
install (TARGETS RinSum DESTINATION bin)

