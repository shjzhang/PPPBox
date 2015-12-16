# apps/difftools/CMakeLists.txt

add_executable(rowdiff rowdiff.cpp)
target_link_libraries(rowdiff pppbox)
install (TARGETS rowdiff DESTINATION bin)

add_executable(rnwdiff rnwdiff.cpp)
target_link_libraries(rnwdiff pppbox)
install (TARGETS rnwdiff DESTINATION bin)

add_executable(rmwdiff rmwdiff.cpp)
target_link_libraries(rmwdiff pppbox)
install (TARGETS rmwdiff DESTINATION bin)

add_executable(ficdiff ficdiff.cpp)
target_link_libraries(ficdiff pppbox)
install (TARGETS ficdiff DESTINATION bin)

add_executable(ephdiff ephdiff.cpp)
target_link_libraries(ephdiff pppbox)
install (TARGETS ephdiff DESTINATION bin)

add_executable(clkdiff clkdiff.cpp)
target_link_libraries(clkdiff pppbox)
install (TARGETS clkdiff DESTINATION bin)

add_executable(sp3diff sp3diff.cpp)
target_link_libraries(sp3diff pppbox)
install (TARGETS sp3diff DESTINATION bin)

add_executable(rnxdiff rnxdiff.cpp)
target_link_libraries(rnxdiff pppbox)
install (TARGETS rnxdiff DESTINATION bin)

