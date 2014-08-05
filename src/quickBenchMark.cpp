/// Parses
#include "parser.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

/// Searches for and returns a new a map of friend ID to name
using Parser = json::Parser<const char*>; 

/// Start the parser off in a dict. Fast forward to the 'friends' entry
void findFriends(Parser& p) {
  do {
    if (!p.findNextAttribute())
      p.handleError("Hit end of the object containing friends");
    std::string attrName = p.readNextAttribute();
    if (attrName == "friends")
      break;
    attrName.clear();
    p.consumeOneValue();
  } while (p.doIHaveMoreAttributes());
}

std::map<int, std::string> readFriends(Parser &p) {
  // Input is of the format [{"id": 4, "name": "joe"}]
  std::map<int, std::string> output;
  std::string attrName;
  auto readFriend = [&](Parser::JSONType type) {
    // read the {"id": 4, "name": "joe"} object
    if (type != Parser::object)
        p.handleError("Expected each friend to be an object");
    int id;
    std::string name;
    bool haveOne = false;
    auto readFriendElement = [&](std::string, Parser::JSONType type) {
      // Read either "id": 4 .. or "name": "joe"
      switch (type) {
      case Parser::number:
        id = p.readNumber<int>();
      case Parser::string:
        name = p.readString();
      default:
        p.handleError("Expected 'name' to be a string, and 'id' to be an int");
      };
      if (haveOne) {
        output.insert({id, name});
        haveOne = false;
      } else {
        haveOne = true;
      }
    };
    p.readObject(readFriendElement);
  };
  // Read the outter array
  p.readArray(readFriend);
  return output;
}

int main(int, char**) {
  // Load the input into memory
  std::ifstream f("benchmark.json");
  std::stringstream input;
  input << f.rdbuf();
  const char* start = input.str().c_str();
  const char* end = input.str().c_str() + input.str().size();
  Parser p(start, end);
  Parser::JSONType t = p.getNextType(); // Consume the '[' at the start
  if (t != Parser::array)
    return 1;
  t = p.getNextType(); // Consume the '{' at the start
  if (t != Parser::object)
    return 2;
  findFriends(p);
  // Read the friends dict
  std::map<int, std::string> friends = readFriends(p);
  const std::string expected("Dixie Sims");
  if (friends[1] != expected) {
    std::cerr << "Didn't read the right friend" << std::endl;
    return 3;
  }
  return 0;
}
