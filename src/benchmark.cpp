/// A quick benchmark program. Reads test_input and extracts on value from it
#define NO_LOCATIONS
#include "json.hpp"
#include "speed.hpp"

#include <fstream>
#include <iostream>
#include <sstream>


int main(int, char **) {
  // Load the input into memory
  constexpr const char* filename = "benchmark.json";
  std::ifstream f(filename);
  if (!f) {
    std::cerr << "Couldn't find file: " << filename << std::endl;
    return 1;
  }
  using namespace json::fast;
  std::stringstream input;
  input << f.rdbuf();
  const std::string& inString = input.str();
  const char* start = inString.c_str();
  const char* end = start + inString.size();
  // Parse it and print out check the friend's name 10k times
  const JSON::String expectedName("DixieSims");
  int retval = 0;
  using Parser = Parser<const char*>;
  for (long i = 0; i < 10000; ++i) {
    Parser p(start, end);
    json::fast::JSON data = std::move(read(p));
    const JList &outter = data;
    const JList &friends = outter[0]["friends"];
    for (const JSON &myFriend : friends) {
      if (myFriend["id"] == 1) {
        if (static_cast<const JSON::String&>(myFriend["name"]) != expectedName) {
          ++retval;
          break;
        }
      }
    }
  }
  std::cout << retval;
  return 0;
}
