/// Parser exception
#pragma once

#include <type_traits>
#include <cstring>

namespace json {

// An iterator may provide an 'int row; int col;' that specify the row and
// column of the input text that it's currently reading. If it does, we'll
// report that info when we throw the exception.
namespace location_checker {

/// If we can compile this function, the iterator has location information
template <typename T>
int checkForLocation(const T& iter) {
  int row = iter.row;
  int col = iter.col;
  return row + col;
}

struct fail {};
template <typename T> struct do_test : std::true_type {};
template <> struct do_test<fail> : std::false_type {};

template <typename T> struct can_call {
private:
  template <typename X>
  static auto check(const X &x)
      -> decltype(checkForLocation(x)); // Has location
  static fail check(...);               // Has no location
public:
  using type = decltype(check(std::declval<T>()));
};

template <typename T>
struct has_location : do_test<typename can_call<T>::type> {};

}

template <typename T>
constexpr bool has_location() {
  return location_checker::has_location<T>::value;
}

struct ParserError : std::runtime_error {
public:
  ParserError(const char* msg) : std::runtime_error(msg) {}
  ParserError(std::string msg) : std::runtime_error(msg) {}
};

/// tparam P the Parser specialization that we refer to
class LocatingParserError : public ParserError {
private:
  static std::string make_msg(const char* msg, int row, int col) {
    std::stringstream result;
    result << msg << " at row " << row << " col " << col;
    // return msg + " at " + location;
    return result.str();
  }
public:
   LocatingParserError(const char* msg, int row, int col)
       : ParserError(make_msg(msg, row, col)), row(row), col(col)  {}
  const int row;
  const int col;
};

template <bool B, typename T=void>
using enable_if = typename std::enable_if<B, T>::type;

template <typename T>
enable_if<has_location<T>> throwError(const char* msg, const T& iter) { throw LocatingParserError(msg, iter.row, iter.col); }

template <typename T>
enable_if<!has_location<T>> throwError(const char* msg, const T&) { throw ParserError(msg); }

}
