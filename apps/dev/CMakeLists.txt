# apps/dev/CMakeLists.txt

add_executable(ppp ppp.cpp)
target_link_libraries(ppp pppbox)
install (TARGETS ppp DESTINATION bin)

add_executable(pppar pppar.cpp)
target_link_libraries(pppar pppbox)
install (TARGETS pppar DESTINATION bin)


