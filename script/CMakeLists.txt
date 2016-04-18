# script/CMakeLists.txt

# If on UNIX, do these
if (UNIX)
	EXECUTE_PROCESS( COMMAND sh -c "chmod u+x ${CMAKE_CURRENT_SOURCE_DIR}/*.sh")
	INSTALL(PROGRAMS get_data.sh DESTINATION bin)
	INSTALL(PROGRAMS get_rnx.sh DESTINATION bin)
	INSTALL(PROGRAMS get_eph.sh DESTINATION bin)
endif (UNIX)
