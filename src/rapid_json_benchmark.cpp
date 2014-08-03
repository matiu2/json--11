#include <fstream>
#include <iostream>
#include <sstream>

#include "rapidjson/document.h"		// rapidjson's DOM-style API

using namespace rapidjson;

int main(int, char**)
{
  // Load the input into memory
  std::ifstream f("benchmark.json");
  std::stringstream input;
  input << f.rdbuf();
  const std::string expected("Dixie Sims");
  int retVal = 0;
  for (long i = 0; i < 10000; ++i) {
    Document document;
	  if (document.Parse<0>(input.str().c_str()).HasParseError())
      return 1;
    assert(document.isArray());
    auto &obj = document[SizeType(0)];
    assert(obj.isObject());
    const auto& friends = obj["friends"];
    assert(friends.IsArray());
    for (Value::ConstValueIterator i=friends.Begin(); i != friends.End(); ++i) {
      const auto& myFriend = *i;
      const auto &id = myFriend["id"];
      assert(id.isInt());
      if (id.GetInt() == 1) {
        const auto &name = myFriend["name"];
        assert(name.isString());
        if (name.GetString() == expected) {
          ++retVal;
          break;
        }
      }
    }
  }
  std::cout << retVal;
  return 0;
}
