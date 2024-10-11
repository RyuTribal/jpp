cmake_minimum_required(VERSION 3.16)
project("jpp")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

if(WIN32)
    set(LLVM_DIR "${CMAKE_SOURCE_DIR}/vendor/llvm/lib/cmake/llvm")
    add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX)
    add_definitions(-DPLATFORM_UNIX)
    find_program(CLANG clang REQUIRED)

    if(NOT CLANG)
        message(FATAL_ERROR "Clang is required")
    endif()
else()
    find_program(CLANG clang REQUIRED)

    if(NOT CLANG)
        message(FATAL_ERROR "Clang is required")
    endif()
endif()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories("src")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

file(GLOB_RECURSE SRC_FILES src/*.c)
message(STATUS "C Source files: ${SRC_FILES}")

add_executable(jpp_compiler ${SRC_FILES})

llvm_map_components_to_libnames(llvm_libs support core irreader native)
target_link_libraries(jpp_compiler ${llvm_libs})