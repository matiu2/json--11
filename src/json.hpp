#pragma once

#include "json_class.hpp"
#include "mapper.hpp"
#include "parser.hpp"
#ifndef NO_LOCATIONS
#include "locatingIterator.hpp"
#endif

#include <istream>
#include <iterator>
#include <algorithm>


namespace json {

JSON read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  JSONParser<Iterator> parser(Iterator(in), Iterator(), skipOverErrors);
  return read(parser);
}

JSON read(const std::string &in, bool skipOverErrors = false) {
  JSONParser<decltype(in.cbegin())> parser(in.cbegin(), in.cend(), skipOverErrors);
  return read(parser);
}

JSON read(const char *begin, const char *end, bool skipOverErrors = false) {
  JSONParser<const char*> parser(begin, end, skipOverErrors);
  return read(parser);
}

using StreamIterator = std::istream_iterator<char>;
#ifndef NO_LOCATIONS
using LocStreamIterator = LocatingIterator<StreamIterator>;
#else
using LocStreamIterator = StreamIterator;
#endif

std::pair<JSON, LocStreamIterator> readWithPos(std::istream &in, bool skipOverErrors = false) {
  JSONParser<StreamIterator> parser(StreamIterator(in), StreamIterator(), skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

using StringIterator = std::string::const_iterator;
#ifndef NO_LOCATIONS
using LocStringIterator = LocatingIterator<StringIterator>;
#else
using LocStringIterator = StringIterator;
#endif

JSON readWithPos(const std::string &in, bool skipOverErrors = false) {
  JSONParser<decltype(in.cbegin())> parser(in.cbegin(), in.cend(), skipOverErrors);
  return read(parser);
}

}
