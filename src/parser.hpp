
#line 1 "/home/matiu/projects/json++11/src/json.rl"
/* Ragel file for parsing json.
*/

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

#ifndef NO_LOCATIONS
#include "locatingIterator.hpp"
#endif

namespace json {


#line 33 "/home/matiu/projects/json++11/src/json.rl"


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
template <typename T = const char *, typename A=std::allocator<char>> class Parser {
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
  using MyType = Parser<T>;
  using iterator = T;
  using Error = ParserError<MyType>;
  using String = std::basic_string<char, char_traits<char>, A>;
#ifndef NO_LOCATIONS
  friend class ParserError<MyType>;
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
  char scanWSFor() {
    while ((p != pe) && (p != eof)) {
      switch (*p++) {
      case 9:
      case 10:
      case 13:
      case ' ':
        continue;
      case c:
        return c;
      default:
#ifndef NO_LOCATIONS
        throw Error(
            std::string("Couldn't find '") + c + "' to signify the start of an attribute value",
            p.row, p.col);
#else
        throw Error(
            std::string("Couldn't find '") + c + "' to signify the start of an attribute value");
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

  /// Searches though whitespace for a @a c meaning the start of an attribute
  /// name or @e meaning the 'end' of the range
  template<char c, char e>
  char scanWSFor() {
    while ((p != pe) && (p != eof)) {
      switch (*p++) {
      case 9:
      case 10:
      case 13:
      case ' ':
        continue;
      case c:
        return c;
      case e:
        return e;
      default:
#ifndef NO_LOCATIONS
        throw Error(
            std::string("Couldn't find '") + c + "' to signify the start of an attribute value",
            p.row, p.col);
#else
        throw Error(
            std::string("Couldn't find '") + c + "' to signify the start of an attribute value");
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
  Parser(T json, T end, bool skipOverErrors = false)
      : p(json), pe(end), eof(pe), skipOverErrors(skipOverErrors) {}
  Parser(const Parser &original)
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
    
#line 427 "/home/matiu/projects/json++11/src/json.rl"
    int startState =
      
#line 422 "/home/matiu/projects/json++11/src/parser.hpp"
1
#line 429 "/home/matiu/projects/json++11/src/json.rl"
    ;
    int cs = startState; // Current state
    
#line 428 "/home/matiu/projects/json++11/src/parser.hpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 45 )
		goto tr0;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr2;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
#line 7 "/home/matiu/projects/json++11/src/number.rl"
	{
        #ifdef DEBUG
        std::cout << "setNegative" << std::endl;
        #endif
        intIsNeg = true;
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 456 "/home/matiu/projects/json++11/src/parser.hpp"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr2;
	goto st0;
tr2:
#line 13 "/home/matiu/projects/json++11/src/number.rl"
	{
        gotAtLeastOneDigit = true;
        intPart *= 10;
        intPart += *p - '0';
        #ifdef DEBUG
        std::cout << "recordInt " << *p << " - " << intPart << std::endl; 
        #endif
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 475 "/home/matiu/projects/json++11/src/parser.hpp"
	switch( (*p) ) {
		case 46: goto st3;
		case 69: goto st4;
		case 101: goto st4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr2;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto st0;
tr3:
#line 21 "/home/matiu/projects/json++11/src/number.rl"
	{
        intPart *= 10;
        intPart += *p - '0';
        --expPart1; // the 'actual' end exponent will be way at the end
        #ifdef DEBUG
        std::cout << "recordDecimal " << *p << " - " << intPart << " - " << expPart1 << std::endl; 
        #endif
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 506 "/home/matiu/projects/json++11/src/parser.hpp"
	switch( (*p) ) {
		case 69: goto st4;
		case 101: goto st4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	switch( (*p) ) {
		case 43: goto tr4;
		case 45: goto tr4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto st0;
tr4:
#line 29 "/home/matiu/projects/json++11/src/number.rl"
	{
        if (*p == '-')
            expIsNeg = true;
        #ifdef DEBUG
        std::cout << "setExpNeg " << expIsNeg << std::endl;
        #endif
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 539 "/home/matiu/projects/json++11/src/parser.hpp"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto st0;
tr5:
#line 36 "/home/matiu/projects/json++11/src/number.rl"
	{
        expPart2 *= 10;
        expPart2 += *p - '0';
        #ifdef DEBUG
        std::cout << "recordExponent " << *p << " - " << expPart2 << std::endl;
        #endif
    }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 557 "/home/matiu/projects/json++11/src/parser.hpp"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 6: 
	case 7: 
	case 8: 
#line 43 "/home/matiu/projects/json++11/src/number.rl"
	{
        #ifdef DEBUG
        std::cout << "gotNumber " << expIsNeg << " - " << expPart1 << " - " << expPart2 << " - " << intPart << " - " << intIsNeg << " - ";
        #endif
        return makeNumber();
    }
	break;
#line 585 "/home/matiu/projects/json++11/src/parser.hpp"
	}
	}

	_out: {}
	}

#line 433 "/home/matiu/projects/json++11/src/json.rl"

    // The state machine returns, so the code will only get here if it can't
    // parse the string
    if (gotAtLeastOneDigit) return makeNumber();
    else handleError("Couldn't read a number");
    return N();
  }

  String readString() {
      String output;
      readString(output);
      return output;
  }

  template <typename S>
  void readString(S& output) {
    
#line 450 "/home/matiu/projects/json++11/src/json.rl"
    int startState = 
#line 612 "/home/matiu/projects/json++11/src/parser.hpp"
1
#line 451 "/home/matiu/projects/json++11/src/json.rl"
    ;
    int cs = startState; // Current state
    wchar_t uniChar = 0;
    int uniCharBytes = 0;
    
#line 620 "/home/matiu/projects/json++11/src/parser.hpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr0:
#line 9 "/home/matiu/projects/json++11/src/string.rl"
	{ output += *p; }
	goto st1;
tr3:
#line 4 "/home/matiu/projects/json++11/src/string.rl"
	{ output += '\b'; }
	goto st1;
tr4:
#line 5 "/home/matiu/projects/json++11/src/string.rl"
	{ output += '\f'; }
	goto st1;
tr5:
#line 6 "/home/matiu/projects/json++11/src/string.rl"
	{ output += '\n'; }
	goto st1;
tr6:
#line 7 "/home/matiu/projects/json++11/src/string.rl"
	{ output += '\r'; }
	goto st1;
tr7:
#line 8 "/home/matiu/projects/json++11/src/string.rl"
	{ output += '\t'; }
	goto st1;
tr11:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            output += uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                output += *i;
        }
    }
#line 9 "/home/matiu/projects/json++11/src/string.rl"
	{ output += *p; }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 696 "/home/matiu/projects/json++11/src/parser.hpp"
	switch( (*p) ) {
		case 34: goto tr1;
		case 92: goto st2;
	}
	goto tr0;
tr1:
#line 63 "/home/matiu/projects/json++11/src/string.rl"
	{
        ++p;
        return;
    }
	goto st5;
tr12:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            output += uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                output += *i;
        }
    }
#line 63 "/home/matiu/projects/json++11/src/string.rl"
	{
        ++p;
        return;
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 758 "/home/matiu/projects/json++11/src/parser.hpp"
	goto st0;
st0:
cs = 0;
	goto _out;
tr13:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            output += uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                output += *i;
        }
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 807 "/home/matiu/projects/json++11/src/parser.hpp"
	switch( (*p) ) {
		case 98: goto tr3;
		case 102: goto tr4;
		case 110: goto tr5;
		case 114: goto tr6;
		case 116: goto tr7;
		case 117: goto tr8;
	}
	goto tr0;
tr8:
#line 10 "/home/matiu/projects/json++11/src/string.rl"
	{
        uniChar = 0;
        uniCharBytes = 0;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 828 "/home/matiu/projects/json++11/src/parser.hpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr9;
	} else
		goto tr9;
	goto st0;
tr9:
#line 14 "/home/matiu/projects/json++11/src/string.rl"
	{
        if (uniCharBytes++ == 4)
            throw std::logic_error("Max unicode char is 32 bits");
        uniChar <<= 4;
        char ch = *p;
        if (ch >= 'a') 
            uniChar += ch - 'a' + 0x0A;
        else if (ch >= 'A')
            uniChar += ch - 'A' + 0x0A;
        else
            uniChar += ch - '0';
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 857 "/home/matiu/projects/json++11/src/parser.hpp"
	switch( (*p) ) {
		case 34: goto tr12;
		case 92: goto tr13;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr9;
	} else
		goto tr9;
	goto tr11;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 457 "/home/matiu/projects/json++11/src/json.rl"

    // The state machine returns, so the code will only get here if it can't
    // parse the string
    handleError("Couldn't read a string");
  }

  /**
  * @brief checks if we have more array items to read
  *
  * @return true if there are more array to read; false if we've hit the end;
  *throw's for unexpected hit
  */
  bool doIHaveMoreArray() { return doIHaveMore<']'>(); }

  /// Finds the next attribute in a dictionary. Puts us just after the '"' of
  /// the attribute name, or at the '}' at the end of a dict. @returns true if
  /// it finds the attribute; false if we're at the end of the dict
  bool findNextAttribute() {
    constexpr char end('}');
    char c = scanWSFor<'"', end>();
    return (c != end);
  }

  /**
  * @brief While reading an object .. get the next attribute name
  */
  bool readNextAttribute(String& output) {
    readString(output);
    scanWSFor<':'>();
  }

  /**
  * @brief While reading an object .. get the next attribute name
  */
  String readNextAttribute() {
    String output = readString();
    scanWSFor<':'>();
    return output;
  }

  /**
    * Consumes and throws away one value
    **/
  void consumeOneValue() {
    switch (getNextType()) {
    case Parser::null:
      readNull();
      return;
    case Parser::boolean:
      readBoolean();
      return;
    case Parser::array:
      do {
        consumeOneValue();
      } while (doIHaveMoreArray());
      return;
    case Parser::object:
      do {
        if (!findNextAttribute())
          break;
        readNextAttribute();
        consumeOneValue();
      } while (doIHaveMoreObject());
      return;
    case Parser::number:
      readNumber<long double>();
      return;
    case Parser::string:
      readString();
      return;
    case Parser::HIT_END:
      return;
    case Parser::ERROR:
      return; // Code should never hit here
    };
  }

  /**
  * Reads through whitespace, return true if it hits @a separator first, false
  * if it hits @a end first.
  * If it hits the separator, it consumes it, so you can readily read the next
  * value.
  *
  * If it hits anything other than separator, whitespace, or end, it calls
  * handleError and returns 'false' if not configured to throw exceptions.
  * (see  handleError() for more info)
  *
  * @tparam end The character that means we hit the end, no more to come
  * @tparam separator The character that means we have more to come
  *
  * @return returns true if we can expect more input, false if we just hit the
  *end
  */
  template <char end, char separator = ','> bool doIHaveMore() {
    struct BadChar {};
    try {
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
          throw BadChar();
        }
      }
    } catch (BadChar) {
      handleError(std::string("Expected a '") + separator + "' or a '" + end +
                  "' but hit a '" + *p + "' instead");
      return false;
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
  bool doIHaveMoreAttributes() { return doIHaveMore<'}'>(); }
  /// Use just after reading the start of an object. @returns true if there are any object attributes
  bool doIHaveMoreObject() { return doIHaveMore<'}', '"'>(); }

  /// Returns the pointer to the json we are parsing
#ifndef NO_LOCATIONS
  LocatingIterator<T> json() const { return p; }
#else
  T json() const { return p; }
#endif

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
    // Full object
    do {
      if (!findNextAttribute())
        break;
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
    } while (doIHaveMoreAttributes());
    return attrsRead;
  }
};

} // namespace json
