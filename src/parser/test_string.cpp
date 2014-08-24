#include <bandit/bandit.h>
#include <sstream>
#include <string>

#include "string.hpp"

using namespace bandit;
using namespace json;

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
      std::string expected = R"(x")";
      auto output = json::parseString(input.begin(), input.end());
      AssertThat(expected, Is().EqualTo(output)); // Needs to be this way around to use the correct snowhouse template
    });

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
