/* Ragel file for parsing json.
*/

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>

#ifndef NO_LOCATIONS
#include "locatingIterator.hpp"
#endif

namespace json {

%%{
    # Harder types

    # string machine
    machine string;
    include string "string.rl";
    main := string;

    # Number machine
    machine number;
    include "number.rl";
    main := number;

}%%

/// Allows lazy evaluation of number types
class NumberInfo {
 private:
  bool _intIsNeg;
  unsigned long long _intPart;
  int _expPart;

 public:
  NumberInfo(bool intIsNeg, unsigned long long intPart, int expPart)
      : _intIsNeg(intIsNeg), _intPart(intPart), _expPart(expPart) {}

  template <typename NumberType>
  NumberType value() const {
    NumberType result = _intPart;
    int expPart = _expPart;
    if (expPart < 0) {
      while (expPart++ < 0) result *= 0.1;
      return _intIsNeg ? -result : result;
    } else {
      while (expPart-- > 0) result *= 10;
      return _intIsNeg ? -result : result;
    }
  }

  operator int() { return value<int>(); }
  operator unsigned int() { return value<unsigned int>(); }
  operator long() { return value<long>(); }
  operator unsigned long() { return value<unsigned long>(); }
  operator long long() { return value<long long>(); }
  operator unsigned long long() { return value<unsigned long long>(); }
  operator float() { return value<float>(); }
  operator double() { return value<double>(); }
  operator long double() { return value<long double>(); }
  operator short() { return value<short>(); }
  operator unsigned short() { return value<unsigned short>(); }
  operator char() { return value<char>(); }
  operator unsigned char() { return value<unsigned char>(); }
};

/// tparam P the Parser specialization that we refer to
template <typename P, typename T=typename P::iterator>
class ParserError : public std::runtime_error {
private:
#ifndef NO_LOCATIONS
  static std::string make_msg(const std::string &msg, int row, int col) {
    std::stringstream result;
    result << msg << " at row " << row << " col " << col;
    // return msg + " at " + location;
    return result.str();
  }
#else
  static std::string make_msg(const std::string &msg) { return msg; }
#endif

 public:
#ifndef NO_LOCATIONS
   ParserError(const std::string &msg, int row, int col)
       : std::runtime_error(make_msg(msg, row, col)), row(row), col(col)  {}
  const int row;
  const int col;
#else
   ParserError(const std::string &msg)
       : std::runtime_error(make_msg(msg)) {}
#endif
};

/** tparam An iterator that returns chars **/
template <typename T = const char *> class Parser {
public:
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
#ifndef NO_LOCATIONS
  using iterator = LocatingIterator<T>;
#else
  using iterator = T;
#endif
  using Error = ParserError<Parser>;
#ifndef NO_LOCATIONS
  friend class ParserError<Parser>;
#endif

private:
// Ragel vars
  iterator p;
  iterator pe;
  iterator eof;
  // Our vars
  bool skipOverErrors;

  /**
  * Tests a null terminated C string. If we reach the null char .. just returns.
  * If something doesn't match, it throws the Error.
  *
  * @param test A null terminated C string to compare 'p' against.
  */
  void checkStaticString(const char *test) {
    while (*test)
      if ((*test++) != (*p++))
        handleError(std::string("Static String '") + test + "' doesn't match");
  }

  /**
   * Returns the number of bytes needed to encode a unicode character in utf8
   */
  int getNumBytes(wchar_t c) {
    int result = 1;
    wchar_t checker = 0x7f;
    if (c <= checker)
      return result;
    checker <<= 4;
    checker |= 0xf;
    result += 1;
    if (c <= checker)
      return result;
    while (c > checker) {
      checker <<= 5;
      checker |= 0x1f;
      ++result;
    }
    return result;
  }

public:
  /// @param json - KEEP THIS STRING ALIVE .. WE DONT COPY IT .. WE USE IT ..
  /// You can't just call it with an ("inplace string")
  Parser(T json, T end, bool skipOverErrors = false)
      : p(json), pe(end), eof(pe), skipOverErrors(skipOverErrors) {}
  Parser(const Parser &original)
      : p(original.p), pe(original.pe), eof(original.eof),
        skipOverErrors(original.skipOverErrors) {}

  /**
  * If 'skipOverErrors' is false, throws a Error, otherwise just returns.
  * Context is automatically passed to the Error object.
  *
  * @param message The error message to show
  */
  void handleError(const std::string &message) {
    if (skipOverErrors) {
      // Skip Forward until we find a new type
      while (p != pe) {
        switch (getNextToken(true)) {
        case number:
          return;
        case ERROR:
          ++p;
          continue;
        case HIT_END:
          // We have to raise an error here. There's no way we can skip
          // forward anymore
#ifndef NO_LOCATIONS
          throw Error(std::string("Hit end: ") + message, p.row, p.col);
#else
          throw Error(std::string("Hit end: ") + message);
#endif
        default:
          return;
        }
      }
    } else {
#ifndef NO_LOCATIONS
      throw Error(message, p.row, p.col);
#else
      throw Error(message);
#endif
    }
  }
  /**
  * Eats whitespace, then tells you the next type found in the JSON stream.
  * It eats the first letter for all types except number.
  *
  * @param returnError defaults to False .. true is used internally when
  *                    getNextToken() is called by handleError()
  * Causes the func to return the ERROR or HIT_END JSONTokens
  * instead of throwing an exception on syntax errors or hitting the end of the
  * stream
  *
  * # Matrix of parameter values for returnErrors and ::skipOverErrors
  *
  *    returnError - skipOverErrors - behaviour on syntax error - on end of
  *                                                               stream
  *      true      -   true         - returns ERROR             - returns
  *                                                               HIT_END
  *      true      -   false        - returns ERROR             - returns
  *                                                               HIT_END
  *      false     -   true         - skips forward             - returns HIT_END
  *                                   until next type
  *                                   and returns that
  *      false     -   false        - throws Error              - throws Error
  *
  * @return the 'JSONToken' found,
  */
  JSONToken getNextToken(bool returnError = false) {
    while ((p != pe) && (p != eof)) {
      switch ((*p)) {
      case 9:
      case 10:
      case 13:
      case ' ':
        ++p;
        continue;
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
      case '9': {
        return number;
      }
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
      default: {
        // If we got here, it's because we hit the end of the stream, or found
        // an unexpected character in the json stream;
        if (returnError)
          // If we are returning error status instead of throwing exceptions
          // If we hit the end return HIT_END
          return ERROR;
        else {
          // If we're not returning errors, either log it then try again, or
          // throw an exception and abort
          handleError(
              "Couldn't Identify next JSON Type"); // Either logs or throws
          ++p;                                     // Carry on searching
        }
      }
      }
    }
    // We hit the end of the stream
    if (!returnError)
      handleError(
          "Hit end of stream while expecting token");
    return HIT_END; // We have to return something .. even if returnError is
                    // false
  }

  void readNull() { checkStaticString("ull"); }

  /**
  * Read a boolean value, either 'true' or 'false'. Assume's you've already
  *found it with 'getNextToken'
  *
  * @return The value of the boolean read.
  */
  bool readBoolean() {
    switch (*p++) {
    case 'r':
      checkStaticString("ue");
      return true;
    case 'a':
      checkStaticString("lse");
      return false;
    default:
      handleError("Couldn't read 'true' nor 'false'");
      // Once the code reaches here, it means that error raising is turned off
      // And we couldn't read a boolean value, so it has skipped us ahead to
      // the start
      // of the next json value. We'll just keep trying to read the boolean
      // until we get it
      // or we stack overflow.
      return readBoolean();
    }
  }

  /**
  * @brief Reads in a number from the json stream
  *
  * @tparam T the type of number we expect to find, defaults to double
  *
  * @return The value of the number we read
  */
  template <typename N = long double> N readNumber() {
    bool intIsNeg = false;          // true if the int part is negative
    bool expIsNeg = false;          // true if the exponent part is negative
    unsigned long long intPart = 0; // The integer part of the number
    int expPart1 = 0; // The inferred exponent part gotten from counting the
                      // decimal digits
    int expPart2 = 0; // The explicit exponent part from the number itself,
                      // added to the inferred exponent part
    bool gotAtLeastOneDigit = false;
    auto makeNumber =
        [&expIsNeg, &expPart1, &expPart2, &intIsNeg, &intPart]() {
      long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
      return NumberInfo(intIsNeg, intPart, expPart);
    };
    %%machine number;
    int startState =
      %%write start;
    ;
    int cs = startState; // Current state
    %%{
        write exec;
    }%%
    // The state machine returns, so the code will only get here if it can't
    // parse the string
    if (gotAtLeastOneDigit) return makeNumber();
    else handleError("Couldn't read a number");
    return N();
  }

  std::string readString() {
    %%machine string;
    int startState = %%write start;
    ;
    int cs = startState; // Current state
    wchar_t uniChar = 0;
    int uniCharBytes = 0;
    std::string output;
    %%{
        write exec;
    }%%
    // The state machine returns, so the code will only get here if it can't
    // parse the string
    handleError("Couldn't read a string");
    return output;
  }

  /// @returns true if you got the type you wanted
  bool expect(JSONToken expected, JSONToken got) {
    if (expected != got) {
      std::stringstream msg;
      if (got == HIT_END)
        msg << "Expected '" << (char)expected << "' but hit the end of input";
      else
        msg << "Expected '" << (char)expected << "' but got '" << (char)got << "' instead";
      handleError(msg.str());
      return false;
    }
    return true;
  }

  /// @returns true if you got the type you wanted
  bool expectAnyRealType(JSONToken got) {
    switch (got) {
    case null:
    case boolean:
    case array:
    case object:
    case number:
    case string:
      return true;
    default: {
      std::stringstream msg;
      if (got == HIT_END)
        msg << "Expected any real json type but hit the end of input";
      else
        msg << "Expected any real json type but got '" << (char)got << "' instead";
      handleError(msg.str());
    }
    };
    return false;
  }

  /**
    * Consumes and throws away one value
    * @returns the type that it consumed
    **/
  JSONToken consumeOneValue(JSONToken alreadyGotType = HIT_END) {
    JSONToken result =
        alreadyGotType == HIT_END ? getNextToken() : alreadyGotType;
    switch (result) {
    case JSONToken::null:
      readNull();
      return result;
    case JSONToken::boolean:
      readBoolean();
      return result;
    case JSONToken::array: {
      readArray([this](JSONToken t){ consumeOneValue(t); });
      return result;
    }
    case JSONToken::object: {
      auto throwAwayAttr = [](std::string &&) {};
      readObject(throwAwayAttr, [this](JSONToken t){ consumeOneValue(t); });
      return result;
    }
    case JSONToken::number:
      readNumber<long double>();
      return result;
    case JSONToken::string:
      readString();
      return result;
    case JSONToken::HIT_END:
      return result;
    case JSONToken::ERROR:
    case JSONToken::COMMA:
    case JSONToken::COLON:
    case JSONToken::ARRAY_END:
    case JSONToken::OBJECT_END:
      return result; // Code should never hit here
    };
  }

  /// Returns the pointer to the json we are parsing
#ifndef NO_LOCATIONS
  LocatingIterator<T> json() const { return p; }
#else
  T json() const { return p; }
#endif

  /// Convenience function to read an array
  /// @onVal will be read when the stream is ready to read a value. The function
  ///        needs to consume that value from the stream.
  void readArray(std::function<void(JSONToken)> onVal) {
    while (true) {
      JSONToken next = getNextToken();
      if (next == ARRAY_END)
          // We're done
          break;
      // Read the value
      if (!expectAnyRealType(next))
        break;
#ifndef NDEBUG
      iterator b4 = p;
      onVal(next);
      assert(b4 != p); // The event handler *must* consume the value (call
                       // consumeOneValue() if you don't want the result)
#else
      onVal(next);
#endif

      // Read the comma or the end of the array
      next = getNextToken();
      if (next == ARRAY_END)
          // We're done
          break;
      if (!expect(COMMA, next))
        // We needed a comma to continue without error
        break;
    }
  }

  /// Convenience function to read an object.
  /// @onAttribute will be called when an attribute name is read
  /// @onVal will be read when the stream is ready to read a value. The function
  ///        needs to consume that value from the stream.
  void readObject(std::function<void(std::string &&)> onAttribute,
                  std::function<void(JSONToken)> onVal) {
    while (true) {
      JSONToken next = getNextToken();
      // Read the attribute name or the end of the object
      if (next == OBJECT_END)
        // We're done
        break;
      if (!expect(string, next))
        break;
      #ifndef NDEBUG
      std::string attrName = readString();
      iterator b4 = p;
      onAttribute(std::move(attrName));
      assert(b4 == p); // onAttribute must not consume anything
      #else
      onAttribute(readString());
      #endif
      // Read the ':' separator
      if (!expect(COLON, getNextToken()))
        break;
      // Read the value
      next = getNextToken();
      if (!expectAnyRealType(next))
        break;
      #ifndef NDEBUG
      b4 = p;
      onVal(next);
      assert(b4 != p); // onVal must consume one value
      #else
      onVal(next);
      #endif
      // Read the comma or the end of the object
      next = getNextToken();
      if (next == OBJECT_END)
        // We're done
        break;
      if (!expect(COMMA, next))
        // We needed a comma to continue without error
        break;
    }
  }
};

} // namespace json
