project(json++11)

cmake_minimum_required(VERSION 2.8)

add_definitions( -std=c++11 -Wall -Wextra )

include(cmake/get_bandit.cmake)

enable_testing()

option(LOCATIONS "Provide a row+col location when a parsing error is hit" true)
if(NOT LOCATIONS)
  add_definitions(-DNO_LOCATIONS)
endif()

# All tests go here
#add_definitions( -fno-access-control ) # Allow access to private members for tests
include_directories(${BANDIT_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(examples)
