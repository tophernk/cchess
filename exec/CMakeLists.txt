add_executable(excchess cchess.c)
target_link_libraries(excchess cchess)

add_executable(cchess_runner cchess_runner.c)
target_link_libraries(cchess_runner cchess)
