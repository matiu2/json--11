#pragma once
/// Include this header file after/instead of json.hpp to use custom allocators
/// to speed everything up.

#include "json.hpp"

namespace json {

struct Pool {
  char *pool;
  char *pos;
  const char *end;
  Pool(size_t size) : pool(new char[size]), pos(pool), end(pool + size) {}
  ~Pool() { delete[] pool; }
  inline char *allocate(size_t n) {
    if (pos + n >= end)
      throw std::range_error("Cannot allocate");
    char *result = pos;
    pos += n;
    return result;
  }
};

struct PoolAlloc : public std::allocator<char> {
  static Pool pool;

  inline char *allocate(size_t n, void *hint = 0) {
    std::cout << "allocating " << n << " hint: " << hint << std::endl;
    char *result = pool.allocate(n);
    std::cout << (void *)result << std::endl;
    return result;
  }
  inline void deallocate(char *p, size_type n) {
    std::cout << "de-allocating " << n << " pointer: " << (void *)p
              << std::endl;
    // Do nothing
  }

  static void init(size_t size) { Pool pool = Pool(size); }
};

namespace fast {
using JSON = BasicJSON<PoolAlloc>;
using JList = BasicJList<PoolAlloc>;
using JMap = BasicJMap<PoolAlloc>;
template <typename T>
using Parser = json::Parser<T, PoolAlloc>;
template <typename T>
JSON read(Parser<T> &parser, typename Parser<T>::JSONToken alreadyGotType = Parser<T>::JSONToken::HIT_END) {
  return json::read<T, PoolAlloc>(parser, alreadyGotType);
}
}

}
