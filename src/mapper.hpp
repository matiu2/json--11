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
      return parser.readBoolean();
    case JSONParser<T>::array: {
      JList contents;
      while (parser.doIHaveMoreArray()) contents.push_back(read(parser));
      return contents;
    }
    case JSONParser<T>::object: {
      JMap contents;
      while (parser.doIHaveMoreObject()) {
        std::string attrName = parser.readNextAttribute();
        contents.insert({attrName, std::move(read(parser))});
      }
      return contents;
    }
    case JSONParser<T>::number:
      return parser.readNumber();
    case JSONParser<T>::string:
      return parser.readString();
    case JSONParser<T>::HIT_END:
    case JSONParser<T>::ERROR:
    default:
      // Should never get here
      throw std::logic_error("Unexpected error while parsing JSON");
  }
}
}
