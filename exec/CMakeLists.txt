add_executable(excchess cchess.c)
target_link_libraries(excchess cchess ccbot)

add_executable(cchess_runner cchess_runner.c)
target_link_libraries(cchess_runner cchess ccbot)

add_executable(cchessserver tcpserver.c)
target_link_libraries(cchessserver cchesstcpserver)

add_executable(cchessclient tcpclient.c)
target_link_libraries(cchessclient cchesstcpclient)
