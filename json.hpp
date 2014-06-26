#pragma once

#include "src/json_class.hpp"
#include "src/mapper.hpp"
#include "src/parser.hpp"
#include <istream>
#include <iterator>
#include <algorithm>

namespace json {

JSON read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  JSONParser<Iterator> p(Iterator(in), Iterator(), skipOverErrors);
  return read(p);
}

JSON read(const std::string &in, bool skipOverErrors = false) {
  JSONParser<std::string::const_iterator> p(in.cbegin(), in.cend(),
                                            skipOverErrors);
  return read(p);
}

JSON read(std::string&& in, bool skipOverErrors = false) {
  JSONParser<std::string::const_iterator> p(in.cbegin(), in.cend(),
                                            skipOverErrors);
  return read(p);
}


JSON read(const char *begin, const char *end, bool skipOverErrors = false) {
  JSONParser<const char*> p(begin, end, skipOverErrors);
  return read(p);
}

}
