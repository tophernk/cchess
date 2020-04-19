cmake_minimum_required(VERSION 3.15)
project(cchess C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
add_library(position position.c)
add_library(piece piece.c)
add_library(config config.c path_node.c print.c)

add_executable(cchess cchess.c position.c print.c config.c piece.c path_node.c)
