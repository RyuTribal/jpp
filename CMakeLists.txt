cmake_minimum_required(VERSION 3.16)
project("jpp")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories("src")

file(GLOB_RECURSE SRC_FILES src/*.c)
message(STATUS "Source files: ${SRC_FILES}")

add_executable(jpp_compiler ${SRC_FILES})