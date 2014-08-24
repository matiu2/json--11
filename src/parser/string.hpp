/// Parses a json string
#pragma once

namespace json {

/// Most basic string parser. Calls back functions for each token/block that it finds.
template <typename Iterator, typename Traits = iterator_traits<Iterator>>
void ParseString(Iterator &p, const Iterator &pe,
                 std::function<void(char)> recordUnchangedChar,
                 std::function<void(char)> recordChar,
                 std::function<void(w_char_t)> recordUnicode) {
  /// Handle the 4 digits of a unicode char
  auto handleUnicode = [&]() {
    wchar_t uniChar = 0;
    int uniCharNibbles = 0;
    while (p != pe) {
      if (uniCharNibbles == sizeof(wchar_t))
        throw std::logic_error("Max unicode char is 32 bits");
      uniChar <<= 4;
      char ch = *p++;
      if ((ch >= 'a') && (ch <= 'f'))
        uniChar += ch - 'a' + 0x0A;
      else if ((ch >= 'A') && (ch <= 'F'))
        uniChar += ch - 'A' + 0x0A;
      else if ((ch >= '0') && (ch <= '9'))
        uniChar += ch - '0';
      else
        break;
      ++uniCharNibbles;
    }
    // We didn't get any unicode digits
    if (uniCharNibbles == 0) {
      throw std::logic_error("\\u with no hex after it");
    }
    if (uniCharNibbles == 0) {
      throw std::logic_error("\\u with no hex after it");
    }
  };
  /// Handle the escaped character after the backslash
  auto handleEscape = [&]() {
    switch (*++p) {
    case 'b':
      recordChar('\b');
      break;
    case 'f':
      recordChar('\f');
      break;
    case 'n':
      recordChar('\n');
      break;
    case 'r':
      recordChar('\r');
      break;
    case 't':
      recordChar('\t');
      break;
    case 'u':
      recordUnicode(readUnicode());
      break;
    default:
      recordUnchangedChar(*p);
      break;
    };
  };
  // Main outter string parsing loop
  while (p != pe) {
    switch (*p) {
      case '\\':
        handleEscape();
        break;
      case '"':
        return;
      default:
        recordChar(*p);
    }
  }
}

template <typename T, typename Traits = iterator_traits<T>>
class StringParser : public BaseParser<T, Traits> {
public:
  using Base = BaseParser<T, Traits>;
  using iterator = typename Base::iterator;
  using ErrorHandler = typename Base::ErrorHandler;
  /// These are the tokens that we expect when parsing a number
  enum Token {
    escape = '\\', // literal
    END = '"',     // either '"' or p == pe
    normal = 'a'   // Anything else
  };
  // The token when parsing an \escape sequence
  enum EscapeToken {
    backspace = 'b',
    formfeed = 'f',
    newline = 'n',
    carriage_return = 'r',
    tab = 't',
    unicode = 'u',
    literal = 'x', // Anything else
    END = '"'
  };
  enum UnicodeToken {
    digit = '0',       // 0..9 or a..z
    END_OF_TOKEN = '"' // Anything else
  };

protected:
  inline Token getOutterToken() const {
    switch (*p) {
    case '\\':
      ++p;
      return escape;
    case '"':
      return END;
    default:
      return normal;
    }
    return END;
  }
  inline getEscapeToken() const {
    switch (*p) {
      switch
        'b' : return backspace;
      switch
        'f' : return formfeed;
      switch
        'n' : return newline;
      switch
        'r' : return carriage_return;
      switch
        't' : return tab;
      switch
        'u' : return unicode;
      switch
        'u' : return unicode;
    default:
      return literal;
    };
  }
  inline void readOutter() {
    switch (*p) {
    case '\\':
      state = escape;
      ++p;
    case '"':
      throw Done();
    default:
      recordChar(*p);
    };
  }
public:
  inline StringParser(iterator p, iterator pe, ErrorHandler onErr={})
      : Base(p, pe, onErr) {}
  inline void run(std::string& output) {
    enum { outter, escape, unicode } state = outter;
    while (p != pe) {
      switch (state) {
        case outter:
        case escape {
          switch (*p) {
            backspace = 'b',
            formfeed = 'f',
            newline = 'n',
            carriage_return = 'r',
            tab = 't',
            unicode = 'u',
            default;


          };
        }
      }
      switch (t = getNextToken()) {

      }
    }

  }


};

}
