cmake_minimum_required(VERSION 3.0)
project(argparse)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/argparse/include
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/optional
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/variant/include
)

# Flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") # -ggdb3 -O0

# Shared library
add_library(argparse SHARED
    argparse/src/argparse.cpp
    argparse/src/option.cpp
    argparse/src/options.cpp
    argparse/src/parser.cpp
    argparse/src/variant.cpp
)

# Sample
add_executable(sample "sample/main.cpp")
target_link_libraries(sample argparse)

# Tests
file(GLOB SOURCES "test/*.cpp")
add_executable(tests ${SOURCES})
target_link_libraries(tests argparse)
target_include_directories(tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/catch2
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)
