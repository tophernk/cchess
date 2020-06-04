include_directories(request service)
add_subdirectory(request)
add_subdirectory(service)

add_executable(moveclient moveclient.c)
target_link_libraries(moveclient requestmove)

add_executable(moveserver moveserver.c)
target_link_libraries(moveserver movesrv requesteval)