include_directories(../.. /usr/local/Cellar/cmocka/1.1.5/include)
find_library(CMOCKA cmocka /usr/local/Cellar/cmocka/1.1.5/lib)

add_executable(ccbot_test ccbot_test.c)
add_test(ccbot_test ccbot_test)
target_link_libraries(ccbot_test cchess ccbot ${CMOCKA})
