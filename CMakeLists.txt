project(json++11)

cmake_minimum_required(VERSION 2.8)

add_definitions( -std=c++1y -Wall -Wextra )

include(cmake/get_bandit.cmake)

enable_testing()

option(LOCATIONS "Provide a row+col location when a json parsing error is hit" true)
if(NOT LOCATIONS)
  add_definitions(-DNO_LOCATIONS)
endif()

find_library(CPP c++) # clang/llvm c++ library that supports trivially_assignable

# All tests go here
include_directories(${BANDIT_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(examples)
