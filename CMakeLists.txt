cmake_minimum_required(VERSION 3.28)
project(mergeable_heap C)

set(CMAKE_C_STANDARD 11)

# Add a definition based on what you want to compile
add_definitions(-DTESTS)

add_executable(mergeable_heap
        10_2_solution.c
        tests.c)
