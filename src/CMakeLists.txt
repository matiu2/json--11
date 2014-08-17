project(src)

add_executable(tests tests.cpp)
add_executable(benchmark benchmark.cpp)
file(COPY benchmark.json benchmark.py DESTINATION .)
add_executable(quickBenchmark quickBenchmark.cpp)
add_test(tests tests)

find_file(RAPID_JSON_DOCUMENT rapidjson/document.h)
if (RAPID_JSON_DOCUMENT)
  add_executable(rapid_json_benchmark rapid_json_benchmark.cpp)
endif()
