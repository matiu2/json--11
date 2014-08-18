/// Parses a json string
#pragma once

namespace json {

/// Records the location of a string inside the json flow
template <typename T>
void getUnicode(T& p) {
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

template <typename T>
void endUnicode(T& p) {
  /*
     UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     0000 0000-0000 007F   0xxxxxxx
     0000 0080-0000 07FF   110xxxxx 10xxxxxx
     0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

     0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
  */
  int numBytes =
      getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
  if (numBytes == 1) {
    output += uniChar;
  } else {
    std::vector<char> bytes;
    for (int i = 1; i < numBytes; ++i) {
      char byte = uniChar & 0x3f; // Only encoding 6 bits right now
      byte |= 0x80;               // Make sure the high bit is set
      bytes.push_back(byte);
      uniChar >>= 6;
    }
    // The last byte is special
    char mask = 0x3f >> (numBytes - 2);
    char byte = uniChar & mask;
    char top = 0xc0;
    for (int i = 2; i < numBytes; ++i) {
      top >>= 1;
      top |= 0x80;
    }
    byte |= top;
    bytes.push_back(byte);
    // Output it
    for (auto i = bytes.rbegin(); i != bytes.rend(); ++i)
      output += *i;
  }
}

/// Returns true if Traits is an random access iterator (can give us stuff to read )
template <typename Traits>
constexpr bool is_random_access_iterator() {
  return std::is_base_of<std::random_access_iterator_tag, typename Traits::iterator_category>::value;
}


/// Records the location of a string inside the json flow
template <typename T, typename Traits = iterator_traits<T>>
struct string {
  using iterator = T;
  using iterator_traits = Traits;

  static_assert(is_forward_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_input_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<iterator_traits::value_type, char>, "We only work on char input");

  iterator begin;
  iterator end;

  // Most efficient size function
  enable_if<is_random_access_iterator<T>(), iterator_traits::difference_type> size() const {
    return end - begin;
  }

  // Much less efficient size function
  enable_if<!is_random_access_iterator<T>(), size_t> size() const {
    iterator i = begin;
    size_t result = 0;
    while (i != pe) {
      ++i;
      ++result;
    }
    return result;
  }

};

template <typename Iterator, typename Out, typename Traits = iterator_traits<Iterator>>
void ParseString(Iterator p, Iterator pe, Out& o, std::function<void(char)> recordChar, std::function<void(w_char)> recordUnicode) {
  auto handleUnicode = [&]() {

  };
  auto handleEscape = [&]() {
    switch (*++p) {
      case 'b': recordChar('\b'); break;
      case 'f': recordChar('\f'); break;
      case 'n': recordChar('\n'); break;
      case 'r': recordChar('\r'); break;
      case 't': recordChar('\t'); break;
      case 'u': handleUnicode(); break;
      default:
        recordChar(*p);
        break;
    };
  }
  while (p != pe) {
    switch (*p) {
      case '\\':
       
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
