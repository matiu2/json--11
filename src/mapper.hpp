/** Maps the parsed values into json_class(s) **/
#include "parser.hpp"
#include "json_class.hpp"

#include <memory>

namespace json {


/// Recursive function that reads from a json string and returns a JSON object
template <typename T, typename A = std::allocator<char>>
json::JSON<A> read(Parser<T, A> &parser) {
  using JSON = json::JSON<A>;
  using Parser = json::Parser<T, A>;
  switch (parser.getNextType()) {
    case Parser::null:
      parser.readNull();
      return {};
    case Parser::boolean:
      return JSON(parser.readBoolean(), 1);
    case Parser::array: {
      typename JSON::JList contents;
      do
        contents.push_back(read(parser));
      while (parser.doIHaveMoreArray());
      return contents;
    }
    case Parser::object: {
      typename JSON::JMap contents;
      do {
        if (!parser.findNextAttribute())
          break;
        typename JSON::String attrName(parser.readNextAttribute());
        contents.insert({attrName, std::move(read(parser))});
      } while (parser.doIHaveMoreAttributes());
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
