/// Base template class of all JSON parsers
#pragma once

#include "error.hpp"

#include <type_traits>
#include <iterator>

namespace json {

////////////////////////////////////////////////////////////////////////////////
// Type support functions
////////////////////////////////////////////////////////////////////////////////

/// Returns true if Traits is a forward iterator (can move forward)
template <typename Traits>
constexpr bool is_forward_iterator() {
  return std::is_base_of<std::forward_iterator_tag, typename Traits::iterator_category>::value;
}

/// Returns true if Traits is an input iterator (can give us stuff to read )
template <typename Traits>
constexpr bool is_input_iterator() {
  return std::is_base_of<std::input_iterator_tag, typename Traits::iterator_category>::value;
}

/// Returns true if T is copy assignable
template <typename T>
constexpr bool is_copy_assignable() {
  return std::is_copy_assignable<T>::value;
}

/// Returns true if T and Y are the same type
template <typename T, typename Y>
constexpr bool is_same() {
  return std::is_same<T, Y>::value;
}

////////////////////////////////////////////////////////////////////////////////
// Actual classes
////////////////////////////////////////////////////////////////////////////////

/// The base of all JSON Parsers
template <typename T, typename Traits = std::iterator_traits<T>>
class BaseParser {
public:
  using iterator = T;
  using iterator_traits = Traits;

  static_assert(is_input_iterator<iterator_traits>(), "T must be a input iterator");
  static_assert(is_forward_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<iterator_traits::value_type, char>, "We only work on char input");

  using ErrorHandler = std::function<void(std::string)>; // A callback function for when we encounter an error
protected:
  iterator p;  /// Current location in the input data
  iterator pe; /// End of the input data
  ErrorHandler handleError;
public:
  inline BaseParser(iterator begin, iterator end, ErrorHandler onErr = {})
      : p(begin), pe(end), handleError(onErr) {
    if (!onErr)
      onErr = [this](std::string msg) { json::throwError(msg, p); };
  }
};

}
