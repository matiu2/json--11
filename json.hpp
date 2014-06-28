#pragma once

#include "src/json_class.hpp"
#include "src/mapper.hpp"
#include "src/parser.hpp"
#include "src/locatingIterator.hpp"
#include <istream>
#include <iterator>
#include <algorithm>


namespace json {

JSON read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  Iterator start = Iterator(in);
  Iterator end;
  JSONParser<LocatingIterator<Iterator>> p(locator(start), locator(end), skipOverErrors);
  return read(p);
}

JSON read(const std::string &in, bool skipOverErrors = false) {
  JSONParser<LocatingIterator<decltype(in.cbegin())>> p(locator(in.cbegin()), locator(in.cend()),
                                            skipOverErrors);
  return read(p);
}

JSON read(std::string&& in, bool skipOverErrors = false) {
  JSONParser<LocatingIterator<std::string::const_iterator>> p(locator(in.cbegin()), locator(in.cend()),
                                            skipOverErrors);
  return read(p);
}


JSON read(const char *begin, const char *end, bool skipOverErrors = false) {
  JSONParser<LocatingIterator<const char*>> p(begin, end, skipOverErrors);
  return read(p);
}

}
