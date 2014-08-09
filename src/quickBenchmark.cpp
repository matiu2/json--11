/// Parses
#include "parser.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include <bitset>
#include <fstream>
#include <iostream>

/// Searches for and returns a new a map of friend ID to name
using Parser = json::Parser<const char*>;

std::pair<int, std::string> readFriend(Parser &p) {
  // The attributes we read from the friend dict
  int id = 0;
  std::string name;
  std::bitset<2> attrs;

  // Called to read the friend's ID from the parser
  auto getId = [&](Parser::JSONToken t) {
    p.expect(Parser::JSONToken::number, t);
    id = p.readNumber<int>();
    attrs.set(0);
  };
  // Called to read the friend's name from the parser
  auto getName = [&](Parser::JSONToken t) {
    p.expect(Parser::JSONToken::string, t);
    name = p.readString();
    attrs.set(1);
  };
  // This points to eiter of the above event handlers
  enum {needId, needName} nextCall;
  // When we read an attribute, set up the next value reader
  auto onAttribute = [&](std::string &&attrName) {
    if (attrName == "id")
      nextCall = needId;
    else if (attrName == "name")
      nextCall = needName;
  };
  // Calls the appropriate value reader (id or name)
  auto onVal = [&](Parser::JSONToken t) {
    switch (nextCall) {
    case needId:
      getId(t);
      break;
    case needName:
      getName(t);
      break;
    };
  };
  p.readObject(onAttribute, onVal);
  if (!attrs.all())
    p.handleError("Expected friend to have both an 'id' and a 'name'");
  return make_pair(id, name);
}

void readFriends(Parser& p, std::map<int, std::string>& output) {
  auto onFriendFound = [&](Parser::JSONToken t) {
    p.expect(Parser::JSONToken::object, t);
    output.insert(readFriend(p));
  };
  p.readArray(onFriendFound);
}

/// Start the parser off in a dict. Fast forward to the 'friends' entry
void findFriends(Parser &p, std::map<int, std::string>& output) {
  std::string lastAttrName;
  auto onAttribute = [&](std::string &&attrName) { lastAttrName = attrName; };
  auto onVal = [&](Parser::JSONToken t) {
    if (lastAttrName == "friends") {
      readFriends(p, output);
    } else {
      p.consumeOneValue(t);
    }
  };
  p.readObject(onAttribute, onVal);
}

int main(int, char**) {
  // Load the input into memory
  std::ifstream f("benchmark.json");
  std::stringstream input;
  input << f.rdbuf();
  const char* start = input.str().c_str();
  const char* end = input.str().c_str() + input.str().size();
  std::string myFriend;
  for (int i=0; i < 10000; ++i) {
    // Parse the json
    Parser p(start, end);
    // Consume the '[' at the start
    auto t = p.getNextToken();
    if (t != Parser::array)
      return 1;
    // Consume the '{' at the start
    t = p.getNextToken();
    if (t != Parser::object)
      return 2;
    // Find the Friends dict
    std::map<int, std::string> friends;
    findFriends(p, friends);
    myFriend = friends[1];
  }
  // Print the name of friend 1
  std::cout << myFriend << std::endl;
  return 0;
}
