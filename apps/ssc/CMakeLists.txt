# apps/filetools/CMakeLists.txt
include_directories("../../lib/AppFrame" "../../lib/CommandLine")
add_executable(ssc2msc ssc2msc.cpp)
target_link_libraries(ssc2msc ../../pppbox)
install (TARGETS ssc2msc DESTINATION bin)

