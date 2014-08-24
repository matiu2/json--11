#include <bandit/bandit.h>
#include <sstream>
#include <string>

#include "string.hpp"

using namespace bandit;
using namespace json;

namespace std {

// Just for snowhouse 'Equals' to work
template <typename T>
bool operator ==(const std::string& a, const json::string_reference<T>& b) {
  return b == a;
}

}

go_bandit([]() {

  std::stringstream output;

  describe("String Parser", [&]() {

    it("1.1. Can be empty", [&]() {
      std::string input = R"(")";
      json::string_reference<std::string::iterator> output = json::parseString(input.begin(), input.end());
      AssertThat(output.begin, Equals(output.end));
    });

    it("1.2. Can be a single char", [&]() {
      std::string input = R"(x")";
      std::string expected = "x";
      auto output = json::parseString(input.begin(), input.end());
      AssertThat(expected, Is().EqualTo(output)); // Needs to be this way around to use the correct snowhouse template
    });

    it("1.3. Can be several chars", [&]() {
      std::string input = R"(abcdefg")";
      std::string expected = "abcdefg";
      std::string expected2 = "(1241142";
      auto output = json::parseString(input.begin(), input.end());
      AssertThat(output, Is().EqualTo(expected));
    });
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
