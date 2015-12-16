# apps/checktools/CMakeLists.txt

add_executable(rowcheck rowcheck.cpp)
target_link_libraries(rowcheck pppbox)
install (TARGETS rowcheck DESTINATION bin)

add_executable(rmwcheck rmwcheck.cpp)
target_link_libraries(rmwcheck pppbox)
install (TARGETS rmwcheck DESTINATION bin)

add_executable(rnwcheck rnwcheck.cpp)
target_link_libraries(rnwcheck pppbox)
install (TARGETS rnwcheck DESTINATION bin)

add_executable(ficcheck ficcheck.cpp)
target_link_libraries(ficcheck pppbox)
install (TARGETS ficcheck DESTINATION bin)

add_executable(ficacheck ficacheck.cpp)
target_link_libraries(ficacheck pppbox)
install (TARGETS ficacheck DESTINATION bin)

