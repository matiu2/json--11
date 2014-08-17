#pragma once
#include "base.hpp"

namespace JSON {

/// Allows lazy evaluation of number types
class NumberInfo {
private:
  bool _intIsNeg;
  unsigned long long _intPart;
  int _expPart;

public:
  NumberInfo(bool intIsNeg, unsigned long long intPart, int expPart)
      : _intIsNeg(intIsNeg), _intPart(intPart), _expPart(expPart) {}

  template <typename NumberType> NumberType value() const {
    NumberType result = _intPart;
    int expPart = _expPart;
    if (expPart < 0) {
      while (expPart++ < 0)
        result *= 0.1;
      return _intIsNeg ? -result : result;
    } else {
      while (expPart-- > 0)
        result *= 10;
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

template <typename T, typename Traits = iterator_traits<T>,
          typename available = enable_if_t<
              is_base_of<forward_iterator_tag,
                         typename Traits::iterator_category>::
                  value // Iterator must be at least a forward iterator
              > class NumberParser {
public:
  enum NumberToken = {digit = 0, positive = '+', negative = '-',
                      dot = '.', exponent = 'e', somethingElse = ' ',
                      END = 'X'};

private:
  bool intIsNeg = false;          // true if the int part is negative
  bool expIsNeg = false;          // true if the exponent part is negative
  unsigned long long intPart = 0; // The integer part of the number
  int expPart1 = 0; // The inferred exponent part gotten from counting the
                    // decimal digits
  int expPart2 = 0; // The explicit exponent part from the number itself,
                    // added to the inferred exponent part
  bool gotAtLeastOneDigit = false;
  inline NumberInfo makeNumber() {
    long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
    return {intIsNeg, intPart, expPart};
  }
  inline NumberToken getNextToken() {
    switch (*p) {
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
      return digit;
    case '-':
      return negative;
    case '+':
      return positive;
    case '.':
      return dot;
    case 'e':
    case 'E':
      return exponent;
    default:
      return somethingElse;
    }
  }
  /// Records a single integer
  void recordInt() {
    gotAtLeastOneDigit = true;
    intPart *= 10;
    intPart += *p - '0';
    ++p;
  }
  void recordDecimal {
    intPart *= 10;
    intPart += *p - '0';
    --expPart1; // the 'actual' end exponent will be way at the end
  }
  void setExpNeg {
    if (*p == '-')
      expIsNeg = true;
  }
  void recordExponent {
    expPart2 *= 10;
    expPart2 += *p - '0';
  }
  /// Records the integer part (before the '.' and before the 'e')
  /// @returns the token that stopped us recording the integer part
  NumberToken readIntegerPart() {
    // Check that we have some input
    if (p == pe)
      handleError("No number found. At end of input");
    // Read the first digit
    NumberToken token = getNextToken();
    switch (token) {
    case digit:
      recordInt();
    case negative:
      setNegative();
    default:
      handleError("Expected a digit or a '-'");
    };
    // Read the rest of the integer part
    while (p != pe) {
      switch (token = getNextToken()) {
      digit:
        recordInt();
      dot:
      exponent:
        ++p;
        return token;
      negative:
        handleError("Didn't expect a '-' in the middle of an integer");
      negative:
        handleError("Didn't expect a '+' in the middle of an integer");
      default:
        return token;
      };
    }
    return END;
  }

  public:
    NumberParser(iterator p, iterator pe, ErrorHandler onErr)
        : BaseParser(p, pe, onErr) {}
    NumberInfo run() {
      NumberToken token = readIntegerPart();
      switch (token) {
        case dot:
          token = readDecimalPart();
        case exponent:
          token = readExponentPart();
        case END:
        case somethingElse:
          return makeNumber();
        default:
          assert("Should never reach here");
      }
    }
  }

  readDigitOrNegative();
  // parse the string
  if (gotAtLeastOneDigit)
    return makeNumber();
  else
    handleError("Couldn't read a number");
  return N();
}
};
}
