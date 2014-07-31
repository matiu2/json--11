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
#include <memory>


namespace json {

template <typename A=std::allocator<char>>
JSON read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  Parser<Iterator, A> parser(Iterator(in), Iterator(), skipOverErrors);
  return read(parser);
}

template <typename A=std::allocator<char>>
JSON read(const std::string &in, bool skipOverErrors = false) {
  Parser<std::string::const_iterator, A> parser(in.cbegin(), in.cend(), skipOverErrors);
  return read(parser);
}

template<typename T, typename A=std::allocator<char>>
JSON read(T begin, T end, bool skipOverErrors = false) {
  Parser<T, A> parser(begin, end, skipOverErrors);
  return read(parser);
}

template<typename T, typename A=std::allocator<char>>
istream& operator >>(istream& i, BasicJSON<T>& j) {
  j = read<A>(i);
  return i;
}

using StreamIterator=std::istream_iterator<char>;
#ifndef NO_LOCATIONS
using LocStreamIterator = LocatingIterator<StreamIterator>;
#else
using LocStreamIterator = StreamIterator;
#endif
std::pair<JSON, LocStreamIterator> readWithPos(std::istream &in, bool skipOverErrors = false) {
  Parser<StreamIterator> parser(StreamIterator(in), StreamIterator(), skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
using LocStringIterator = LocatingIterator<std::string::const_iterator>;
#else
using LocStringIterator = std::string::const_iterator;
#endif
std::pair<JSON, LocStringIterator> readWithPos(const std::string &in, bool skipOverErrors = false) {
  Parser<std::string::const_iterator> parser(in.cbegin(), in.cend(), skipOverErrors);
  JSON&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
template <typename T>
std::pair<JSON, LocatingIterator<T>> readWithPos(T begin, T end,
                                                 bool skipOverErrors = false) {
#else
template <typename T>
std::pair<JSON, T> readWithPos(T begin, T end, bool skipOverErrors = false) {
#endif
  Parser<T> parser(begin, end, skipOverErrors);
  JSON &&result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}
}
