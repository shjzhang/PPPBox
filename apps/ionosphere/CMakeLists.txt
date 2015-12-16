# apps/ionosphere/CMakeLists.txt

add_executable(IonoBias IonoBias.cpp)
target_link_libraries(IonoBias pppbox)
install (TARGETS IonoBias DESTINATION bin)

add_executable(TECMaps TECMaps.cpp VTECMap.cpp)
target_link_libraries(TECMaps pppbox)
install (TARGETS TECMaps DESTINATION bin)

