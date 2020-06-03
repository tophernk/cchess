add_library(movesrv STATIC move_service.c)
target_link_libraries(movesrv cchess pthread)