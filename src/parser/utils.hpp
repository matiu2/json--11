/// Utility function and types used by the parser components
#pragma once

namespace json {

template <bool B, typename T=void>
using enable_if = typename std::enable_if<B, T>::type;

template <typename T>
using remove_pointer = typename std::remove_pointer<T>::type;

template <typename T, typename Y>
constexpr bool is_base_of() {
  return std::is_base_of<T, Y>::value;
}

template <typename T>
constexpr bool is_copy_assignable() {
  return std::is_copy_assignable<T>::value;
}

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

/// Returns true if T and Y are the same type
template <typename T, typename Y>
constexpr bool is_same() {
  return std::is_same<T, Y>::value;
}

/// Returns true if Traits is a random access iterator (can give us stuff to read )
template <typename Traits>
constexpr bool is_random_access_iterator() {
  return is_base_of<std::random_access_iterator_tag, typename Traits::iterator_category>();
}

/// This whole namespace is dedicated to checking if a type has a .row and .col int attributes at compile time
namespace location_checker {

/// If we can compile this function, the iterator has location information
template <typename T> int checkForLocation(const T &iter) {
  int row = iter.row;
  int col = iter.col;
  return row + col;
}

struct fail {}; // The return type of a check that doesn't have what we need
template <typename T> struct do_test : std::true_type {};
template <> struct do_test<fail> : std::false_type {};

/// A type function that determines if we can call checkForLocation
template <typename T> struct can_call {
private:
  template <typename X>
  static auto check(const X &x)
      -> decltype(checkForLocation(x)); // Has location
  static fail check(...);               // Has no location
public:
  using type = decltype(check(std::declval<T>()));
};

/// Has has_location<T>::value == true if T has .row and .col attributes that
/// can be ints
template <typename T>
struct has_location : do_test<typename can_call<T>::type> {};
}

/// A compile time function that returns true if T has .row and .col attributes that can be ints
template <typename T>
constexpr bool has_location() {
  return location_checker::has_location<T>::value;
}


}
