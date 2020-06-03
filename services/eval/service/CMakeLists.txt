add_library(evalsrv STATIC eval_service.c)
target_link_libraries(evalsrv cchess pthread)