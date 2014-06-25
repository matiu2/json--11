#pragma once

#include "src/json_class.hpp"
#include "src/mapper.hpp"
#include "src/parser.hpp"
#include <istream>
#include <iterator>

namespace json {

JSON read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  Iterator begin(in);
  Iterator end;
  JSONParser<Iterator> p(begin, end, skipOverErrors);
  return read(p);
}

JSON read(const std::string &in, bool skipOverErrors = false) {
  JSONParser<std::string::const_iterator> p(in.cbegin(), in.cend(),
                                            skipOverErrors);
  return read(p);
}
}
