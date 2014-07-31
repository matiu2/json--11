/** Maps the parsed values into json_class(s) **/
#include "parser.hpp"
#include "json_class.hpp"

#include <memory>

namespace json {

/// Recursive function that reads from a json string and returns a JSON object
template <typename T, typename A=std::allocator<char>>
BasicJSON<A> read(Parser<T, A> &parser, typename Parser<T, A>::JSONToken alreadyGotType = Parser<T, A>::JSONToken::HIT_END) {
  using Parser = Parser<T, A>;
  using JSONToken = typename Parser::JSONToken;
  using JSON = BasicJSON<A>;
  using JList = BasicJList<A>;
  using JMap = BasicJMap<A>;
  using String = typename Parser::String;
  switch (alreadyGotType == Parser::HIT_END ? parser.getNextToken() : alreadyGotType) {
  case Parser::null:
    parser.readNull();
    return {};
  case Parser::boolean:
    return JSON(parser.readBoolean(), 1);
  case Parser::array: {
    JList contents;
    auto readElement = [&](JSONToken t) {
      contents.push_back(read<T, A>(parser, t));
    };
    parser.readArray(readElement);
    return contents;
  }
  case Parser::object: {
    JMap contents;
    String lastAttrName;
    auto readAttribute = [&](String&& attr) {
      lastAttrName = std::move(attr);
    };
    auto readValue = [&](JSONToken t) {
        contents.insert({lastAttrName, std::move(read<T, A>(parser, t))});
    };
    parser.readObject(readAttribute, readValue);
    return contents;
  }
  case Parser::number:
    return parser.readNumber();
  case Parser::string:
    return JSON(parser.readString().c_str());
  case Parser::HIT_END:
  case Parser::ERROR:
  default:
    // Should never get here
    throw std::logic_error("Unexpected error while parsing JSON");
  }
}
}
