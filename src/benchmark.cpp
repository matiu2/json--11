/// A quick benchmark program. Reads test_input and extracts on value from it
#define NO_LOCATIONS
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int, char **) {
  // Load the input into memory
  std::ifstream f("benchmark.json");
  std::stringstream input;
  input << f.rdbuf();
  std::string instr = input.str();
  const char* start = instr.c_str();
  const char* end = instr.c_str() + instr.size();
  // Parse it and print out check the friend's name 10k times
  const std::string expected("DixieSims");
  int retval = 0;
  for (long i = 0; i < 10000; ++i) {
    json::JSON data = json::read(start, end);
    const json::JList &outter(data);
    const json::JList &friends = outter[0]["friends"];
    for (const json::JSON &myFriend : friends) {
      if (myFriend["id"] == 1) {
        if (static_cast<std::string>(myFriend["name"]) != expected) {
          ++retval;
          break;
        }
      }
    }
  }
  std::cout << retval;
  return 0;
}
