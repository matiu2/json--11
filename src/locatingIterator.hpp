/** An iterator wrapper that records a location in column and row coordinates */
#pragma once

namespace json {

template <typename T> struct LocatingIterator {
  using MyType = LocatingIterator<T>;
  T real;
  int row;
  int col;
  inline MyType &operator++() {
    if (*real == '\n') {
      ++row;
      col = 1;
    } else {
      ++col;
    }
    ++real;
    return *this;
  }
  inline MyType operator++(int) {
    MyType result = *this;
    operator ++();
    return result;
  }
  inline char operator*() { return *real; }
  inline char &operator->() { return *real; }
  inline bool operator !=(const MyType& other) const { return real != other.real; }
  inline bool operator ==(const MyType& other) const { return real == other.real; }
  inline LocatingIterator(T real) : real(real), row(1), col(1) {}
  inline LocatingIterator(const MyType &other) : real(other.real), row(other.row), col(other.col) {}
};

}
