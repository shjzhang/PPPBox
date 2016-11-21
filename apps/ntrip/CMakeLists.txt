# apps/filetools/CMakeLists.txt

SET( LINK_FLAGS "-lpthread")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}" )


add_executable(ntrip ntrip.cpp)
target_link_libraries(ntrip pppbox)
install (TARGETS ntrip DESTINATION bin)

