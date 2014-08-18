#pragma once
#include "base.hpp"

namespace json {

/// Allows lazy evaluation of number types
class NumberInfo {
private:
  bool _intIsNeg;
  unsigned long long _intPart;
  int _expPart;

public:
  NumberInfo(bool intIsNeg=true, unsigned long long intPart=0, int expPart=0)
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

template <typename T, typename Traits = iterator_traits<T>>
class NumberParser : public BaseParser<T, Traits> {
public:
  using Base = BaseParser<T, Traits>;
  using iterator = typename Base::iterator;
  using ErrorHandler = typename Base::ErrorHandler;
  /// These are the tokens that we expect when parsing a number
  enum Token {
    digit = 0, // Any digit 0..9
    // The next 4 tokens are literals
    positive = '+',
    negative = '-',
    dot = '.',
    exponent = 'e',
    END = ' ', // End of the number; either p == pe .. or we hit an unexpected
               // character (like a space for example)
  };

protected:
  iterator& p = Base::p;
  iterator& pe = Base::pe;
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
    // parse the string
    if (gotAtLeastOneDigit) {
      long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
      return {intIsNeg, intPart, expPart};
    } else {
      Base::handleError("Couldn't read a number"); // Might reach here if we find for example, a standalone + or - in the json
    }
  }
  inline Token getNextToken() {
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
      return END;
    }
  }
  /// Records a single integer
  inline void recordInt() {
    gotAtLeastOneDigit = true;
    intPart *= 10;
    intPart += *p - '0';
    ++p;
  }
  inline void recordDecimal() {
    intPart *= 10;
    intPart += *p - '0';
    --expPart1; // the 'actual' end exponent will be way at the end
  }
  inline void setExpNeg() {
    if (*p == '-')
      expIsNeg = true;
  }
  inline void recordExponent() {
    expPart2 *= 10;
    expPart2 += *p - '0';
  }
  /// Records the integer part (before the '.' and before the 'e')
  /// @returns the token that stopped us recording the integer part
  inline Token readIntegerPart() {
    // Check that we have some input
    if (p == pe)
      Base::handleError("No number found. At end of input");
    // Read the first digit
    Token token = getNextToken();
    switch (token) {
    case digit:
      recordInt();
      break;
    case negative:
      intIsNeg = true;
      break;
    default:
      Base::handleError("Expected a digit or a '-'");
    };
    // Read the rest of the integer part
    while (p != pe) {
      switch (token = getNextToken()) {
      case digit:
        recordInt();
        break;
      case dot:
      case exponent:
        ++p;
        return token;
      case negative:
        Base::handleError("Didn't expect a '-' in the middle of a number");
      case positive:
        Base::handleError("Didn't expect a '+' in the middle of a number");
      default:
        return END;
      };
    }
    return END;
  }

  inline Token readDecimalPart() {
    while (p != pe) {
      Token token;
      switch (token = getNextToken()) {
      case digit:
        recordDecimal();
        break;
      case dot:
        Base::handleError("Second '.' found in a number");
      case exponent:
        ++p; // The actual 'e' is not needed when reading the exponent
        return token;
      case negative:
        Base::handleError("Didn't expect a '-' in the middle of a number");
      case positive:
        Base::handleError("Didn't expect a '+' in the middle of a number");
      default:
        return END;
      };
    }
    return END;
  }

  inline Token readExponentPart() {
    while (p != pe) {
      Token token;
      switch (token = getNextToken()) {
      case digit:
        recordExponent();
      case dot:
        Base::handleError("'.' found in a exponent");
      case exponent:
        Base::handleError("Second 'e' for exponent found in number");
      case negative:
        Base::handleError("Didn't expect a '-' in the middle of a number");
      case positive:
        Base::handleError("Didn't expect a '+' in the middle of a number");
      default:
        return END;
      };
    }
    return END;
  }

  NumberInfo inline makeNumber() const {
    long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
    return NumberInfo(intIsNeg, intPart, expPart);
  }


public:
  inline NumberParser(iterator p, iterator pe, ErrorHandler onErr={})
      : Base(p, pe, onErr) {}
  inline NumberInfo run() {
    // The number can be in on of these formats
    // integer
    // integer decimal
    // integer decimal exponent
    // integer exponent

    // Read the 1st part
    Token token = readIntegerPart();  // First part of the number
    // Now we should see a decimal or an exponent part
    bool haveExponent = false;
    switch (token) {
    case dot:
      token = readDecimalPart();            // 2nd part of the number
      break;
    case exponent:
      token = readExponentPart(); 
      haveExponent = true;
      break;
    case END:
      return makeNumber();
    default:
      assert("Should never reach here");
      Base::handleError("Unexpected token in number");
    };

    if (haveExponent) {
      switch (token) {
      case END:
        return makeNumber();
      default:
        assert("Should never get here. All error conditions should have been "
               "handled above");
        Base::handleError("Didn't expect more number tokens after the exponent");
      };
    } else {
      switch (token) {
      case exponent:
        token = readExponentPart();
        break;
      case END:
        return makeNumber();
      default:
        assert("Should never get here. All error conditions should have been "
               "handled above");
        Base::handleError("Unexpected token in number");
      };
    }

    assert("Should never get here"); // We should always return a number above
    return {};
  }
};

}

