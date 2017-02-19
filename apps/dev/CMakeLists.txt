# apps/dev/CMakeLists.txt

add_executable(ppp ppp.cpp)
target_link_libraries(ppp pppbox)
install (TARGETS ppp DESTINATION bin)

add_executable(pppar pppar.cpp)
target_link_libraries(pppar pppbox)
install (TARGETS pppar DESTINATION bin)

#add_executable(pppucar pppucar.cpp)
#target_link_libraries(pppucar pppbox)
#install (TARGETS pppucar DESTINATION bin)

#add_executable(pod pod.cpp)
#target_link_libraries(pod pppbox)
#install (TARGETS pod DESTINATION bin)

add_executable(pppgnss pppgnss.cpp)
target_link_libraries(pppgnss pppbox)
install (TARGETS pppgnss DESTINATION bin)

add_executable(sfppp sfppp.cpp)
target_link_libraries(sfppp pppbox)
install (TARGETS sfppp DESTINATION bin)

add_executable(getFCB GetFCB.cpp)
target_link_libraries(getFCB pppbox)
install (TARGETS getFCB DESTINATION bin)

add_executable(ppprt ppprt.cpp)
target_link_libraries(ppprt pppbox)
install (TARGETS ppprt DESTINATION bin)

