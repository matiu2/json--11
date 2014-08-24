/// Parser exception
#pragma once

#include <type_traits>
#include <cstring>

#include "utils.hpp"

namespace json {

// An iterator may provide an 'int row; int col;' that specify the row and
// column of the input text that it's currently reading. If it does, we'll
// report that info when we throw the exception.

/// The base parser Error, doesn't return any location information
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

// A couple of throwError template functions, that will help choose the correct ParserError class to throw

/// Throws the LocatingParserError
template <typename T>
enable_if<has_location<T>> throwError(const char* msg, const T& iter) { throw LocatingParserError(msg, iter.row, iter.col); }

/// Throws the ParserError
template <typename T>
enable_if<!has_location<T>> throwError(const char* msg, const T&) { throw ParserError(msg); }

}
