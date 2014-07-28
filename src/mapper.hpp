/** Maps the parsed values into json_class(s) **/
#include "parser.hpp"
#include "json_class.hpp"

namespace json {

/// Recursive function that reads from a json string and returns a JSON object
template <typename T>
JSON read(Parser<T> &parser, typename Parser<T>::JSONToken alreadyGotType = Parser<T>::JSONToken::HIT_END) {
  using Parser = Parser<T>;
  using JSONToken = typename Parser::JSONToken;
  switch (alreadyGotType == JSONToken::HIT_END ? parser.getNextToken() : alreadyGotType) {
  case Parser::null:
    parser.readNull();
    return {};
  case Parser::boolean:
    return JSON(parser.readBoolean(), 1);
  case Parser::array: {
    JList contents;
    auto readElement = [&](JSONToken t) {
      contents.push_back(read(parser, t));
    };
    parser.readArray(readElement);
    return contents;
  }
  case Parser::object: {
    JMap contents;
    std::string lastAttrName;
    auto readAttribute = [&](std::string&& attr) {
      lastAttrName = attr;
    };
    auto readValue = [&](JSONToken t) {
        contents.insert({lastAttrName, std::move(read(parser, t))});
    };
    parser.readObject(readAttribute, readValue);
    return contents;
  }
  case Parser::number:
    return parser.readNumber();
  case Parser::string:
    return JSON(parser.readString());
  case Parser::HIT_END:
  case Parser::ERROR:
  default:
    // Should never get here
    throw std::logic_error("Unexpected error while parsing JSON");
  }
}
}
