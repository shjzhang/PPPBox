# apps/clocktools/CMakeLists.txt

add_executable(dallandev dallandev.cpp)
target_link_libraries(dallandev pppbox)
install (TARGETS dallandev DESTINATION bin)

add_executable(ffp ffp.cpp)
target_link_libraries(ffp pppbox)
install (TARGETS ffp DESTINATION bin)

add_executable(mallandev mallandev.cpp)
target_link_libraries(mallandev pppbox)
install (TARGETS mallandev DESTINATION bin)

add_executable(nallandev nallandev.cpp)
target_link_libraries(nallandev pppbox)
install (TARGETS nallandev DESTINATION bin)

add_executable(oallandev oallandev.cpp)
target_link_libraries(oallandev pppbox)
install (TARGETS oallandev DESTINATION bin)

add_executable(ohadamarddev ohadamarddev.cpp)
target_link_libraries(ohadamarddev pppbox)
install (TARGETS ohadamarddev DESTINATION bin)

add_executable(ORDPhaseParser ORDPhaseParser.cpp)
target_link_libraries(ORDPhaseParser pppbox)
install (TARGETS ORDPhaseParser DESTINATION bin)

add_executable(pff pff.cpp)
target_link_libraries(pff pppbox)
install (TARGETS pff DESTINATION bin)

add_executable(rmoutlier rmoutlier.cpp)
target_link_libraries(rmoutlier pppbox)
install (TARGETS rmoutlier DESTINATION bin)

add_executable(scale scale.cpp)
target_link_libraries(scale pppbox)
install (TARGETS scale DESTINATION bin)

add_executable(tallandev tallandev.cpp)
target_link_libraries(tallandev pppbox)
install (TARGETS tallandev DESTINATION bin)

add_executable(TIAPhaseParser TIAPhaseParser.cpp)
target_link_libraries(TIAPhaseParser pppbox)
install (TARGETS TIAPhaseParser DESTINATION bin)

add_executable(trunc trunc.cpp)
target_link_libraries(trunc pppbox)
install (TARGETS trunc DESTINATION bin)

