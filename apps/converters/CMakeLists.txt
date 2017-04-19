# apps/converters/CMakeLists.txt

add_executable(novaRinex novaRinex.cpp)
target_link_libraries(novaRinex pppbox)
install (TARGETS novaRinex DESTINATION bin)

add_executable(ash2mdp ash2mdp.cpp)
target_link_libraries(ash2mdp pppbox)
install (TARGETS ash2mdp DESTINATION bin)

add_executable(ash2xyz ash2xyz.cpp)
target_link_libraries(ash2xyz pppbox)
install (TARGETS ash2xyz DESTINATION bin)


add_executable(navcorr2Sp3 navcorr2Sp3.cpp)
target_link_libraries(navcorr2Sp3 pppbox)
install (TARGETS navcorr2Sp3 DESTINATION bin)

