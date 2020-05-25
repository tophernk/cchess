cmake_minimum_required(VERSION 3.7)
project(cchess C)
enable_testing()
set(CMAKE_C_STANDARD 11)

option(CLANG "clang compiler" ON)
option(GCC "gcc compiler" OFF)

if (CLANG)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -Wall -Wextra")
endif (CLANG)

if (GCC)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif (GCC)

include_directories(lib server)
add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(exec)
add_subdirectory(test)
