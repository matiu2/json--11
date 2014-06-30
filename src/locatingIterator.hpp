/** An iterator wrapper that records a location in column and row coordinates */
#pragma once

namespace json {

template <typename T> struct LocatingIterator {
  using MyType = LocatingIterator<T>;
  T real;
  int row;
  int col;
  MyType &operator++() {
    if (*real == '\n') {
      ++row;
      col = 1;
    } else {
      ++col;
    }
    ++real;
    return *this;
  }
  MyType operator++(int) {
    MyType result = *this;
    operator ++();
    return result;
  }
  char operator*() { return *real; }
  char &operator->() { return *real; }
  bool operator !=(const MyType& other) const { return real != other.real; }
  bool operator ==(const MyType& other) const { return real == other.real; }
  LocatingIterator(T real) : real(real), row(1), col(1) {}
  LocatingIterator(const MyType &other) : real(other.real), row(other.row), col(other.col) {}
};

}
