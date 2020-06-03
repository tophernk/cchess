include_directories(request service)
add_subdirectory(request)
add_subdirectory(service)

add_executable(evalclient evalclient.c)
target_link_libraries(evalclient requesteval)

add_executable(evalserver evalserver.c)
target_link_libraries(evalserver evalsrv)