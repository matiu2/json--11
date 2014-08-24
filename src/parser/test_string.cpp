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

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
