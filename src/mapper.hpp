/** Maps the parsed values into json_class(s) **/
#include "parser.hpp"
#include "json_class.hpp"

namespace json {

/// Recursive function that reads from a json string and returns a JSON object
template <typename T>
JSON read(Parser<T>& parser) {
  switch (parser.getNextType()) {
    case Parser<T>::null:
      parser.readNull();
      return {};
    case Parser<T>::boolean:
      return JSON(parser.readBoolean(), 1);
    case Parser<T>::array: {
      JList contents;
      do
        contents.push_back(read(parser));
      while (parser.doIHaveMoreArray());
      return contents;
    }
    case Parser<T>::object: {
      JMap contents;
      do {
        if (!parser.findNextAttribute())
          break;
        std::string attrName = parser.readNextAttribute();
        contents.insert({attrName, std::move(read(parser))});
      } while (parser.doIHaveMoreAttributes());
      return contents;
    }
    case Parser<T>::number:
      return parser.readNumber();
    case Parser<T>::string:
      return JSON(parser.readString());
    case Parser<T>::HIT_END:
    case Parser<T>::ERROR:
    default:
      // Should never get here
      throw std::logic_error("Unexpected error while parsing JSON");
  }
}
}
