/// Base template class of all JSON parsers
#pragma once

#include "error.hpp"

#include <type_traits>
#include <iterator>

namespace json {

template <typename Traits>
constexpr bool is_forward_iterator() {
  return std::is_base_of<std::forward_iterator_tag, typename Traits::iterator_category>::value;
}

template <typename T>
constexpr bool is_copy_assignable() {
  return std::is_copy_assignable<T>::value;
}

template <typename T, typename Y>
constexpr bool is_same() {
  return std::is_same<T, Y>::value;
}

template <typename T, typename Traits = std::iterator_traits<T>>
class BaseParser {
public:
  using iterator = T;
  using iterator_traits = Traits;

  static_assert(is_forward_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<iterator_traits::value_type, char>, "We only work on char input");

  using ErrorHandler = std::function<void(const char*, const T&)>; // A callback function for when we encounter an error
protected:
  iterator p;  /// Current location in the stream
  iterator pe; /// End of the stream
  ErrorHandler handleError;
public:
  BaseParser(iterator begin, iterator end,
             ErrorHandler onErr = throwError)
      : p(begin), pe(end), handleError(onErr) {}
};

}
