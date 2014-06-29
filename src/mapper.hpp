#include "parser.hpp"
#include "json_class.hpp"

/** Uses the parser to create JSON classes **/

namespace json {

/// Recursive function that reads from a json string and returns a JSON object
template <typename T>
JSON read(JSONParser<T>& parser) {
  switch (parser.getNextType()) {
    case JSONParser<T>::null:
      return {};
    case JSONParser<T>::boolean:
      return JSON(parser.readBoolean(), 1);
    case JSONParser<T>::array: {
      JList contents;
      do
        contents.push_back(read(parser));
      while (parser.doIHaveMoreArray());
      return contents;
    }
    case JSONParser<T>::object: {
      JMap contents;
      do {
        std::string attrName = parser.readNextAttribute();
        contents.insert({attrName, std::move(read(parser))});
      } while (parser.doIHaveMoreObject());
      return contents;
    }
    case JSONParser<T>::number:
      return parser.readNumber();
    case JSONParser<T>::string:
      return JSON(parser.readString());
    case JSONParser<T>::HIT_END:
    case JSONParser<T>::ERROR:
    default:
      // Should never get here
      throw std::logic_error("Unexpected error while parsing JSON");
  }
}
}
