add_library(cchess STATIC logger.c position.c piece.c move.c config.c print.c)
add_subdirectory(test)
