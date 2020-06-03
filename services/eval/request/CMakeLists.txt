add_library(requesteval STATIC request_eval.c)
target_link_libraries(requesteval cchess pthread)