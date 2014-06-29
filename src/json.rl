/* Ragel file for parsing json.
*/

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
class JSONNumberInfo {
 private:
  bool _intIsNeg;
  unsigned long long _intPart;
  int _expPart;

 public:
  JSONNumberInfo(bool intIsNeg, unsigned long long intPart, int expPart)
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
  operator short() { return value<short>(); }
  operator unsigned short() { return value<unsigned short>(); }
  operator char() { return value<char>(); }
  operator unsigned char() { return value<unsigned char>(); }
};

/// tparam P the JSONParser specialization that we refer to
template <typename P, typename T=typename P::iterator>
class JSONParserError : public std::runtime_error {
private:
  std::string make_msg(const std::string &msg, int row, int col) {
#ifndef NO_LOCATIONS
    std::stringstream result;
    result << msg << " at row " << row << " col " << col;
    // return msg + " at " + location;
    return result.str();
#else
    return msg;
#endif
  }

 public:
#ifndef NO_LOCATIONS
   JSONParserError(const std::string &msg, int row, int col)
       : std::runtime_error(make_msg(msg, row, col)), row(row), col(col)  {}
  const int row;
  const int col;
#else
   JSONParserError(const std::string &msg)
       : std::runtime_error(make_msg(msg)) {}
#endif
};

/** tparam An iterator that returns chars **/
template <typename T = const char *> class JSONParser {
public:
  enum JSONType {
    null = 'n',
    boolean = 't',
    array = '[',
    object = '{',
    number = '0',
    string = '"',
    HIT_END = 0,
    ERROR = 'x'
  };
  using MyType = JSONParser<T>;
  using iterator = T;
  using Error = JSONParserError<MyType>;
#ifndef NO_LOCATIONS
  friend class JSONParserError<MyType>;
#endif

private:
// Ragel vars
#ifndef NO_LOCATIONS
  LocatingIterator<T> p;
  LocatingIterator<T> pe;
  LocatingIterator<T> eof;
#else
  T p;
  T pe;
  T eof;
#endif
  // Our vars
  bool skipOverErrors;

  /// Searches though whitespace for a @a c meaning the start of an attribute
  /// name
  template<char c>
  void scanWSFor() {
    while ((p != pe) && (p != eof)) {
      switch (*p++) {
      case 9:
      case 10:
      case 13:
      case ' ':
        continue;
      case c:
        return;
      default:
#ifndef NO_LOCATIONS
        throw Error(
            std::string("Couldn't find '") + c + "' to signify the start of an attribute value",
            p.row, p.col);
#else
        throw Error(
            "Couldn't find '" + c + "' to signify the start of an attribute value");
#endif
      }
    }
#ifndef NO_LOCATIONS
    throw Error(std::string("hit end while looking for '") + c + "' to signify the start of an "
                "attribute value",
                p.row, p.col);
#else
    throw Error(std::string("hit end while looking for '") + c + "' to signify the start of an "
                "attribute value");
#endif
  }

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
        switch (getNextType(true)) {
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
  JSONParser(T json, T end, bool skipOverErrors = false)
      : p(json), pe(end), eof(pe), skipOverErrors(skipOverErrors) {}
  JSONParser(const JSONParser &original)
      : p(original.p), pe(original.pe), eof(original.eof),
        skipOverErrors(original.skipOverErrors) {}

  /**
  * Eats whitespace, then tells you the next type found in the JSON stream.
  * It eats the first letter for all types except number.
  *
  * @param returnError defaults to False .. true is used internally when
  *getNextType() is called by handleError()
  *                    Causes the func to return the ERROR or HIT_END JSONTypes
  *instead of throwing an exception on syntax errors or hitting the end of the
  *stream
  *
  * # Matrix of parameter values for returnErrors and ::skipOverErrors
  *
  *    returnError - skipOverErrors - behaviour on syntax error - on end of
  *stream
  *      true      -   true         - returns ERROR             - returns
  *HIT_END
  *      true      -   false        - returns ERROR             - returns
  *HIT_END
  *      false     -   true         - skips forward until next type and returns
  *that - returns HIT_END
  *      false     -   false        - throws Error              - throws Error
  *
  * @return the 'JSONType' found,
  */
  JSONType getNextType(bool returnError = false) {
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
          "Hit end of stream while trying to identify next JSON type"); // Either
                                                                        // logs
                                                                        // or
    // throws
    return HIT_END; // We have to return something .. even if returnError is
                    // false
  }

  void readNull() { checkStaticString("ull"); }

  /**
  * Read a boolean value, either 'true' or 'false'. Assume's you've already
  *found it with 'getNextType'
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
  template <typename N = double> N readNumber() {
    bool intIsNeg = false;          // true if the int part is negative
    bool expIsNeg = false;          // true if the exponent part is negative
    unsigned long long intPart = 0; // The integer part of the number
    int expPart1 = 0; // The inferred exponent part gotten from counting the
                      // decimal digits
    int expPart2 = 0; // The explicit exponent part from the number itself,
                      // added to the inferred exponent part
    bool gotAtLeastOneDigit = false;
    auto makeJSONNumber =
        [&expIsNeg, &expPart1, &expPart2, &intIsNeg, &intPart]() {
      long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
      return JSONNumberInfo(intIsNeg, intPart, expPart);
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
    if (gotAtLeastOneDigit) return makeJSONNumber();
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

  /**
  * @brief checks if we have more array items to read
  *
  * @return true if there are more array to read; false if we've hit the end;
  *throw's for unexpected hit
  */
  bool doIHaveMoreArray() { return doIHaveMore<']'>(); }

  /**
  * @brief While reading an object .. get the next attribute name
  */
  std::string readNextAttribute() {
    scanWSFor<'"'>();
    std::string output = readString();
    scanWSFor<':'>();
    return output;
  }

  /**
    * Consumes and throws away one value
    **/
  void consumeOneValue() {
    switch (getNextType()) {
    case JSONParser::null:
      readNull();
      return;
    case JSONParser::boolean:
      readBoolean();
      return;
    case JSONParser::array:
      while (doIHaveMoreArray()) {
        consumeOneValue();
      }
      return;
    case JSONParser::object:
      while (doIHaveMoreObject()) {
        readNextAttribute();
        consumeOneValue();
      }
      return;
    case JSONParser::number:
      readNumber<int>();
      return;
    case JSONParser::string:
      readString();
      return;
    case JSONParser::HIT_END:
      return;
    case JSONParser::ERROR:
      return; // Code should never hit here
    };
  }

  /**
  * Reads through whitespace, return true if it hits @a separator first, false
  * if it hits @a end first.
  * If it hits anything other than these or whitespace, it return true and
  * doesn't advance to the next char
  *
  * @tparam end The character that means we hit the end, no more to come
  * @tparam separator The character that means we have more to come
  *
  * @return returns true if we can expect more input, false if we just hit the
  *end
  */
  template <char end, char separator = ','> bool doIHaveMore() {
    while ((p != pe) && (p != eof)) {
      switch (*p) {
      case 9:
      case 10:
      case 13:
      case ' ':
        ++p;
        continue;
      case separator:
        ++p;
        return true;
      case end:
        ++p;
        return false;
      default:
        return true;
      }
    }
    handleError(std::string("Expected a '") + separator + "' or a '" + end +
                "' but hit the end of the input");
    return false;
  }

  /**
  * @brief While reading an object .. returns true if there is more to read.
  *Also moves us forward in the parsing.
  *
  * @return true if there is more Object to read
  */
  bool doIHaveMoreObject() { return doIHaveMore<'}'>(); }

  /// Returns the pointer to the json we are parsing
  T json() const { return p; }

  /// This is the type you can pass to 'readObject'
  using Reader = std::function<void()>;
  using TypeReaderPair = std::pair<JSONType, Reader>;
  using ReaderMap = std::map<std::string, TypeReaderPair>;

  /// Reads an object
  /// @a readerMap - a map of attribute name to expected JSONType, reader
  /// function pair.
  /// For expample: { { "name", { JSONType::string, [&parser, &person]() {
  /// person.setName(parser.readString()); } }}}
  /// @returns - the number of attributes read
  size_t readObject(const ReaderMap &readerMap) {
    size_t attrsRead = 0;
    while (doIHaveMoreObject()) {
      std::string nextAttrName = readNextAttribute();
      // See if we expected an attribute with that name
      auto pReader = readerMap.find(nextAttrName);
      if (pReader == readerMap.end()) {
        consumeOneValue();
        continue;
      }
      // See if the type matches what we expected
      JSONType nextTokenType = getNextType();
      auto pair = pReader->second;
      auto expectedType = pair.first;
      auto reader = pair.second;
      if (nextTokenType != expectedType) {
        if (nextTokenType == JSONType::null) {
          readNull();
          // If we expected something and found null,
          // We'll just pretend we didn't find it
          // TODO: possibly make the reader() smarter and able
          //       to be notified about finding null
          continue;
        }
        std::stringstream errMsg;
        errMsg << "When reading attribute " << nextAttrName
               << "I expected a value of type " << expectedType
               << "but got one of type " << nextTokenType;
#ifndef NO_LOCATIONS
        throw Error(errMsg.str(), p.row, p.col);
#else
        throw Error(errMsg.str());
#endif
      }
      reader(); // Actually read in the value to the person object
      ++attrsRead;
    }
    return attrsRead;
  }
};

} // namespace json
