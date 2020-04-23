include_directories(${PROJECT_SOURCE_DIR} /usr/local/Cellar/cmocka/1.1.5/include)
find_library(CMOCKA cmocka /usr/local/Cellar/cmocka/1.1.5/lib)

add_executable(logger_test logger_test.c)
add_test(logger_test logger_test)
target_link_libraries(logger_test cchesslogger ${CMOCKA})

add_executable(position_test position_test.c)
add_test(position_test position_test)
target_link_libraries(position_test lcchess ${CMOCKA})

add_executable(piece_test piece_test.c)
add_test(piece_test piece_test)
target_link_libraries(piece_test lcchess ${CMOCKA})

add_executable(move_test move_test.c)
add_test(move_test move_test)
target_link_libraries(move_test lcchess ${CMOCKA})

add_executable(config_test config_test.c)
add_test(config_test config_test)
target_link_libraries(config_test lcchess ${CMOCKA})