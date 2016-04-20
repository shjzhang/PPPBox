# examples/CMakeLists.txt

add_executable(example1 example1.cpp)
target_link_libraries(example1 pppbox)

add_executable(example2 example2.cpp)
target_link_libraries(example2 pppbox)

add_executable(example3 example3.cpp)
target_link_libraries(example3 pppbox)

add_executable(example4 example4.cpp)
target_link_libraries(example4 pppbox)

add_executable(example5 example5.cpp)
target_link_libraries(example5 pppbox)

add_executable(example6 example6.cpp)
target_link_libraries(example6 pppbox)

add_executable(example7 example7.cpp)
target_link_libraries(example7 pppbox)

add_executable(example8 example8.cpp)
target_link_libraries(example8 pppbox)
install (TARGETS example8 DESTINATION bin)

add_executable(example9 example9.cpp)
target_link_libraries(example9 pppbox)
install (TARGETS example9 DESTINATION bin)

add_executable(example10 example10.cpp)
target_link_libraries(example10 pppbox)
add_executable(example11 example11.cpp)
target_link_libraries(example11 pppbox)

add_executable(example12 example12.cpp)
target_link_libraries(example12 pppbox)

add_executable(example13 example13.cpp)
target_link_libraries(example13 pppbox)

add_executable(example14 example14.cpp)
target_link_libraries(example14 pppbox)
install (TARGETS example14 DESTINATION bin)


add_executable(example15 example15.cpp)
target_link_libraries(example15 pppbox)

#add_executable(example16 example16.cpp)
#target_link_libraries(example16 pppbox)

#add_executable(example17 example17.cpp)
#target_link_libraries(example17 pppbox)

#add_executable(example18 example18.cpp)
#target_link_libraries(example18 pppbox)


if (UNIX)
  execute_process(COMMAND sh -c "chmod u+x ${CMAKE_CURRENT_SOURCE_DIR}/*.sh")
  install (PROGRAMS ppp_example.sh DESTINATION bin/)
  install (PROGRAMS ppp_example_plot.sh DESTINATION bin/)
  install (PROGRAMS pppar_example.sh DESTINATION bin/)
  install (PROGRAMS pppar_example_plot.sh DESTINATION bin/)
endif (UNIX)
