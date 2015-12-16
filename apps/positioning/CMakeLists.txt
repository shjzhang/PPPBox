# apps/positioning/CMakeListt.txt

add_executable(rinexpvt rinexpvt.cpp)
target_link_libraries(rinexpvt pppbox)
install (TARGETS rinexpvt DESTINATION bin)

add_executable(poscvt poscvt.cpp)
target_link_libraries(poscvt pppbox)
install (TARGETS poscvt DESTINATION bin)

add_executable(PRSolve PRSolve.cpp)
target_link_libraries(PRSolve pppbox)
install (TARGETS PRSolve DESTINATION bin)

add_executable(posInterp posInterp.cpp)
target_link_libraries(posInterp pppbox)
install (TARGETS posInterp DESTINATION bin)

add_executable(posmsc posmsc.cpp)
target_link_libraries(posmsc pppbox)
install (TARGETS posmsc DESTINATION bin)

