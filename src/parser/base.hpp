/// Base template class of all JSON parsers
#pragma once

#include "utils.hpp"
#include "error.hpp"

#include <type_traits>
#include <iterator>

namespace json {

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
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<typename iterator_traits::value_type, char>, "We only work on char input");

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
