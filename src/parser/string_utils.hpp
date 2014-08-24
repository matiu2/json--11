/// Holds utilities for the string parser
#pragma once

#include <algorithm>

#include "utils.hpp"

namespace json {

/// Like a string type, but less copying and memory allocation; uses start and
/// end iterators. Falls back to work with forward iterators. No allocators.
template <typename T, typename Traits = std::iterator_traits<T>>
struct string_reference {
  using iterator = T;
  using iterator_traits = Traits;

  static_assert(is_forward_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_input_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<typename iterator_traits::value_type, char>, "We only work on char input");

  iterator begin;
  iterator end;

  // Much less efficient size function, but still works
  typename Traits::difference_type size() const {
    if (is_random_access_iterator<T>) {
      return end - begin;
    } else {
      iterator i = begin;
      typename Traits::difference_type result = 0;
      while (i != end) {
        ++i;
        ++result;
      }
      return result;
    }
  }

  // Compare to ourselves (for sorting in a map)
  bool operator <(const string_reference& other) {
    return std::lexicographical_compare(begin, end, other.begin, other.end);
  }

  // Compare with pretty much anything with a begin(), end()
  template <typename Other>
  bool operator <(const Other& other) const {
    return std::lexicographical_compare(begin, end, other.begin(), other.end());
  }

  // Compare with pretty much anything with a begin(), end()
  template <typename Other>
  bool operator ==(const Other& other) const {
    return std::mismatch(begin, end, other.begin(), other.end()).first == end;
  }

  // Compare with pretty much anything with a begin(), end()
  template <typename Other>
  bool operator !=(const Other& other) const {
    return std::mismatch(begin, end, other.begin(), other.end()).first != end;
  }

  // For conversion to std::basic_string<X>
  template <typename Other>
  operator Other() {
    Other result;
    result.reserve(size());
    auto out = std::back_inserter(result);
    std::copy(begin, end, out);
    return result;
  }
};

}
