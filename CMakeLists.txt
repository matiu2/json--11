project(json++11)

cmake_minimum_required(VERSION 2.8)

add_definitions( -std=c++11 -Wall -Wextra )

enable_testing()

add_subdirectory(src)
