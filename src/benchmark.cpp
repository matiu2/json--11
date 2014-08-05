/// A quick benchmark program. Reads test_input and extracts on value from it
//#define NO_LOCATIONS
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include <memory>

struct LoggingAllocator : std::allocator<char> {
  using Parent = std::allocator<char>;

  char *allocate(size_t size, void* hint = nullptr) {
    char *result = Parent::allocate(size, hint);
    std::cout << (void *)this << " - " << (void *)result << " +" << size;
    return result;
  }

  void deallocate(char *addr, size_t size) {
    std::cout << (void *)this << " - " << (void *)addr << " +" << size;
    Parent::deallocate(addr, size);
  }
};

int main(int, char **) {
  // Load the input into memory
  std::ifstream f("benchmark.json");
  std::stringstream input;
  input << f.rdbuf();
  // Parse it and print out check the friend's name 10k times
<<<<<<< Updated upstream
  const std::string expected("Dixie Sims");
=======
  using JSON = json::JSON<LoggingAllocator>;
  const JSON::String expected("DixieSims");
>>>>>>> Stashed changes
  int retval = 0;
  for (long i = 0; i < 10000; ++i) {
    JSON data = json::read<LoggingAllocator>(input.str());
    const JSON::JList &outter(data);
    const JSON::JList &friends = outter[0]["friends"];
    for (const JSON &myFriend : friends) {
      if (myFriend["id"] == 1) {
<<<<<<< Updated upstream
        if (static_cast<std::string>(myFriend["name"]) == expected) {
=======
        if (static_cast<JSON::String>(myFriend["name"]) != expected) {
>>>>>>> Stashed changes
          ++retval;
          break;
        }
      }
    }
  }
  std::cout << retval;
  return 0;
}
