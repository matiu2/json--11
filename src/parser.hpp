/// JSON parser, lowest level
#pragma once

#include <iterator>
#include <cstring>
#include <type_traits>

#include "parser/base.hpp"

namespace json {

template <typename T, typename Traits = iterator_traits<T>>
class OutterParser : public BaseParser<T, Traits> {
  enum JSONToken {
    HIT_END = 0,
    COMMA = ',',
    COLON = ':',
    null = 'n',
    boolean = 't',
    array = '[',
    ARRAY_END = ']',
    object = '{',
    OBJECT_END = '}',
    number = '0',
    string = '"',
    ERROR = 'x'
  };
  enum StringToken {
    ESCAPE='\\',
    END='"'
  };
  enum StringEscapeToken {
    UNICODE='u',
    NUMBER='0'
  };
protected:
  /// Tests a null terminated C string. If we reach the null char .. just
  /// returns.
  /// If something doesn't match, it throws the Error.
  ///
  /// @param test A null terminated C string to compare 'p' against.
  void checkStaticString(const char *test) {
    while (*test)
      if ((*test++) != (*p++))
        handleError(strcat("Static String '", test, "' doesn't match"));
  }
  /// Skips whitespace
  inline void skipWS() {
    while ((p != pe) && (p != eof)) {
      switch ((*p)) {
      case 9:
      case 10:
      case 13:
      case ' ':
        ++p;
        continue;
      default:
        return;
      }
    }
  }
  /// Gets the next JSONToken and lines you up to read the contents
  inline JSONToken getNextToken() {
    skipWS();
    while ((p != pe) && (p != eof)) {
      switch ((*p)) {
      case '"':
        ++p;
        return string;
      case ',':
        ++p;
        return COMMA;
      case ':':
        ++p;
        return COLON;
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return number;
      case '[':
        ++p;
        return array;
      case ']':
        ++p;
        return ARRAY_END;
      case 'f':
        ++p;
        return boolean;
      case 'n':
        ++p;
        return null;
      case 't':
        ++p;
        return boolean;
      case '{':
        ++p;
        return object;
      case '}':
        ++p;
        return OBJECT_END;
      default:
        // If we got here, it's because we hit the end of the stream, or found
        // an unexpected character in the json stream;
        return ERROR;
      }
    }
  }
  /// Reads the rest of the word 'null'
  inline void readNull() { checkStaticString("ull"); }
  /**
  * Read a boolean value, either 'true' or 'false'. Assume's you've already
  *found it with 'getNextToken'
  *
  * @return The value of the boolean read.
  */
  inline bool readBoolean() {
    switch (*p++) {
    case 'r':
      checkStaticString("ue");
      return true;
    case 'a':
      checkStaticString("lse");
      return false;
    default:
      handleError("Couldn't read 'true' nor 'false'");
    }
  }
  /**
  * @brief Reads in a number from the json stream
  *
  * @tparam T the type of number we expect to find, defaults to double
  *
  * @return The value of the number we read
  */
  template <typename N = long double> N readNumber();public:
};

}
