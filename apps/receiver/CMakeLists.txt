# apps/receiver/CMakeLists.txt

add_executable(rtAshtech AshtechMessage.cpp rtAshtech.cpp ScreenControl.cpp FDStreamBuff.cpp TCPStreamBuff.cpp)
target_link_libraries(rtAshtech pppbox)
install (TARGETS rtAshtech DESTINATION bin)

