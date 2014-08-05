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
JSON<A> read(std::istream &in, bool skipOverErrors = false) {
  using Iterator = std::istream_iterator<char>;
  Parser<Iterator, A> parser(Iterator(in), Iterator(), skipOverErrors);
  return read(parser);
}

template <typename A=std::allocator<char>>
JSON<A> read(const std::string &in, bool skipOverErrors = false) {
  Parser<std::string::const_iterator, A> parser(in.cbegin(), in.cend(), skipOverErrors);
  return read(parser);
}

template<typename T, typename A=std::allocator<char>>
JSON<A> read(T begin, T end, bool skipOverErrors = false) {
  Parser<T, A> parser(begin, end, skipOverErrors);
  return read(parser);
}

template <typename A=std::allocator<char>>
istream& operator >>(istream& i, json::JSON<A>& j) {
  j = read(i);
  return i;
}

using StreamIterator=std::istream_iterator<char>;
#ifndef NO_LOCATIONS
using LocStreamIterator = LocatingIterator<StreamIterator>;
#else
using LocStreamIterator = StreamIterator;
#endif
template <typename A=std::allocator<char>>
std::pair<JSON<A>, LocStreamIterator> readWithPos(std::istream &in, bool skipOverErrors = false) {
  Parser<StreamIterator> parser(StreamIterator(in), StreamIterator(), skipOverErrors);
  JSON<A>&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
using LocStringIterator = LocatingIterator<std::string::const_iterator>;
#else
using LocStringIterator = std::string::const_iterator;
#endif
template <typename A=std::allocator<char>>
std::pair<JSON<A>, LocStringIterator> readWithPos(const std::string &in, bool skipOverErrors = false) {
  Parser<std::string::const_iterator> parser(in.cbegin(), in.cend(), skipOverErrors);
  JSON<A>&& result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}

#ifndef NO_LOCATIONS
template <typename T, typename A=std::allocator<char>>
std::pair<JSON<A>, LocatingIterator<T>> readWithPos(T begin, T end,
                                                 bool skipOverErrors = false) {
#else
template <typename T, typename A=std::allocator<char>>
std::pair<JSON<A>, T> readWithPos(T begin, T end, bool skipOverErrors = false) {
#endif
  Parser<T> parser(begin, end, skipOverErrors);
  JSON<A> &&result(read(parser));
  auto p = parser.json();
  return make_pair(result, p);
}
}
