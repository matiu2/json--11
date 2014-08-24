/// Holds utilities for the string parser
#pragma once

namespace json {

/// Returns true if Traits is a random access iterator (can give us stuff to read )
template <typename Traits>
constexpr bool is_random_access_iterator() {
  return std::is_base_of<std::random_access_iterator_tag, typename Traits::iterator_category>::value;
}

/// Like a string type, but less copying and memory allocation; uses start and
/// end iterators. Falls back to work with forward iterators. No allocators.
template <typename T, typename Traits = iterator_traits<T>>
struct string {
  using iterator = T;
  using iterator_traits = Traits;

  static_assert(is_forward_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_input_iterator<iterator_traits>(), "T must be a forward iterator");
  static_assert(is_copy_assignable<iterator>(), "T must support copying by assignment");
  static_assert(is_same<iterator_traits::value_type, char>, "We only work on char input");

  iterator begin;
  iterator end;

  // Most efficient size function
  enable_if<is_random_access_iterator<T>(), iterator_traits::difference_type> size() const {
    return end - begin;
  }

  // Much less efficient size function
  enable_if<!is_random_access_iterator<T>(), size_t> size() const {
    iterator i = begin;
    size_t result = 0;
    while (i != pe) {
      ++i;
      ++result;
    }
    return result;
  }

};

}
