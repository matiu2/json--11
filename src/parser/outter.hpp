/// The parser for finding the next JSON value type

namespace json {

template <typename iterator,
          typename iterator_traits = std::itererator_traits<iterator>>
JSONToken getNextToken(bool returnError = false) {
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
}

}
