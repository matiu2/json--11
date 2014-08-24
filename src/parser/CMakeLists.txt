project (parser)

add_executable(test_string test_string.cpp)
target_link_libraries(test_string ${CPP})
add_test(test_string test_string)
