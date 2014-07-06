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
  JSONParser<std::string::const_iterator> parser(in.cbegin(), in.cend(), skipOverErrors);
  return read(parser);
}

template<typename T>
JSON read(T begin, T end, bool skipOverErrors = false) {
  JSONParser<T> parser(begin, end, skipOverErrors);
  return read(parser);
}

istream& operator >>(istream& i, json::JSON& j) {
  j = read(i);
  return i;
}

#ifndef NO_LOCATIONS
using StreamIterator=std::istream_iterator<char>;
using LocStreamIterator = LocatingIterator<StreamIterator>;
std::pair<JSON, LocStreamIterator> readWithPos(std::istream &in, bool skipOverErrors = false) {
  JSONParser<StreamIterator> parser(StreamIterator(in), StreamIterator(), skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

using LocStringIterator = LocatingIterator<std::string::const_iterator>;
std::pair<JSON, LocStringIterator> readWithPos(const std::string &in, bool skipOverErrors = false) {
  JSONParser<std::string::const_iterator> parser(in.cbegin(), in.cend(), skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

template<typename T>
std::pair<JSON, LocatingIterator<T>>  readWithPos(T begin, T end, bool skipOverErrors = false) {
  JSONParser<T> parser(begin, end, skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}
#endif


}
