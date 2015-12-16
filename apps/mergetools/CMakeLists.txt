# apps/mergetools/CMakeLists.txt


add_executable(mergeRinObs mergeRinObs.cpp)
target_link_libraries(mergeRinObs pppbox)
install (TARGETS mergeRinObs DESTINATION bin)

add_executable(mergeRinNav mergeRinNav.cpp)
target_link_libraries(mergeRinNav pppbox)
install (TARGETS mergeRinNav DESTINATION bin)

add_executable(mergeRinMet mergeRinMet.cpp)
target_link_libraries(mergeRinMet pppbox)
install (TARGETS mergeRinMet DESTINATION bin)

add_executable(mergeFIC mergeFIC.cpp)
target_link_libraries(mergeFIC pppbox)
install (TARGETS mergeFIC DESTINATION bin)

