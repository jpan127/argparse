cmake_minimum_required(VERSION 3.0)
project(argparse)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/catch2
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/optional
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/variant/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)

# Flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# Sample
add_executable(sample "sample/main.cpp")

# Tests
file(GLOB SOURCES "test/*.cpp")
add_executable(tests ${SOURCES})
