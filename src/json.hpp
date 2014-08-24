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

JSON read(std::istream &in) {
  using Iterator = std::istream_iterator<char>;
  OutterParser<Iterator> parser{Iterator(in), Iterator()};
  return read(parser);
}

JSON read(const std::string &in) {
  OutterParser<std::string::const_iterator> parser(in.cbegin(), in.cend());
  return read(parser);
}

template<typename T>
JSON read(T begin, T end) {
  OutterParser<T> parser(begin, end);
  return read(parser);
}

istream& operator >>(istream& i, json::JSON& j) {
  j = read(i);
  return i;
}

using StreamIterator=std::istream_iterator<char>;
#ifndef NO_LOCATIONS
using LocStreamIterator = LocatingIterator<StreamIterator>;
#else
using LocStreamIterator = StreamIterator;
#endif
std::pair<JSON, LocStreamIterator> readWithPos(std::istream &in) {
  OutterParser<StreamIterator> parser{StreamIterator(in), StreamIterator()};
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
using LocStringIterator = LocatingIterator<std::string::const_iterator>;
#else
using LocStringIterator = std::string::const_iterator;
#endif
std::pair<JSON, LocStringIterator> readWithPos(const std::string &in) {
  OutterParser<std::string::const_iterator> parser(in.cbegin(), in.cend());
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
template <typename T>
std::pair<JSON, LocatingIterator<T>> readWithPos(T begin, T end) {
#else
template <typename T>
std::pair<JSON, T> readWithPos(T begin, T end) {
#endif
  OutterParser<T> parser(begin, end);
  JSON &&result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}
}
