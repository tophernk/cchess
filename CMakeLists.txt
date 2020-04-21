cmake_minimum_required(VERSION 3.7)
project(cchess C)
enable_testing()
set(CMAKE_C_STANDARD 11)

add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
add_link_options(-fprofile-instr-generate -fcoverage-mapping)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
add_library(position position.c)
add_library(piece piece.c)
add_library(config config.c move.c print.c)

add_executable(cchess cchess.c position.c print.c config.c piece.c move.c)
