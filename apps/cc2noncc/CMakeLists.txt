# apps/difftools/CMakeLists.txt

add_executable(cc2noncc cc2noncc.cpp)
target_link_libraries(cc2noncc pppbox)
install (TARGETS cc2noncc DESTINATION bin)
