# workplace/ppp/CMakeLists.txt

if (UNIX)
   EXECUTE_PROCESS(COMMAND sh -c "chmod u+x ${CMAKE_CURRENT_SOURCE_DIR}/*.sh")
   INSTALL(PROGRAMS ppp.sh DESTINATION bin)
endif (UNIX)
