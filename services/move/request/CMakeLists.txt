add_library(requestmove STATIC request_move.c)
target_link_libraries(requestmove cchess pthread)