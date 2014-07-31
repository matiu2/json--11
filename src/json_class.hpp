#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include <memory>

#include "utf82json.hpp"

namespace json {

template <typename A=std::allocator<char>>
struct JSONTraits;

template <typename A = std::allocator<char>, typename traits=JSONTraits<A>> struct BasicJSON;

template <typename A = std::allocator<char>>
using String = std::basic_string<char, std::char_traits<char>, A>;

template <typename A = std::allocator<char>>
using BasicJList = std::vector<BasicJSON<A>>;

template <typename A = std::allocator<char>>
using BasicJMap = std::map<String<A>, BasicJSON<A>>;

template <typename A>
struct JSONTraits {
  using String = json::String<A>;
  using JSON = BasicJSON<std::allocator<char>>;
  using JList = BasicJList<std::allocator<char>>;
  using JMap = BasicJMap<std::allocator<char>>;
};

template <typename A, typename traits> struct BasicJSON {
public:
  enum Type { null, boolean, number, text, map, list };
  using String = typename traits::String;
  using JSON = typename traits::JSON;
  using JList = typename traits::JList;
  using JMap = typename traits::JMap;
private:
  friend std::ostream &operator<<<A>(ostream &s, const JSON &j);
  Type type;
  union Value {
    String as_string;
    long double as_num;
    bool as_bool;
    JMap as_map;
    JList as_list;
    Value() {}
    Value(String val) { new (&as_string) String(val); }
    Value(long double val) : as_num(val) {}
    Value(bool val, int)
        : as_bool(val) {
    } // Need to pass an int to differentiate from as_num constructor
    Value(JMap val) { new (&as_map) JMap(val); }
    Value(JList val) { new (&as_list) JList(val); }
    ~Value() {}
  } value;
  void cleanup() noexcept {
    switch (type) {
    case null:
    case boolean:
    case number:
      break;
    case text:
      value.as_string.~String();
      break;
    case map:
      value.as_map.~JMap();
      break;
    case list:
      value.as_list.~JList();
      break;
    }
    type = null;
  }
  void copyFromOther(const JSON &other) {
    cleanup();
    type = other.type;
    switch (other.type) {
    case null:
      value.as_num = 0;
      break;
    case boolean:
      value.as_bool = other.value.as_bool;
      break;
    case number:
      value.as_num = other.value.as_num;
      break;
    case text:
      new (&value.as_string) String;
      value.as_string = other.value.as_string;
      break;
    case map:
      new (&value.as_map) JMap;
      value.as_map = other.value.as_map;
      break;
    case list:
      new (&value.as_list) JList;
      value.as_list = other.value.as_list;
      break;
    }
  }
  void moveFromOther(JSON &&other) {
    cleanup();
    type = other.type;
    switch (other.type) {
    case null:
      value.as_num = 0;
      break;
    case boolean:
      value.as_bool = other.value.as_bool;
      break;
    case number:
      value.as_num = other.value.as_num;
      break;
    case text:
      new (&value.as_string) String(std::move(other.value.as_string));
      break;
    case map:
      new (&value.as_map) JMap(std::move(other.value.as_map));
      break;
    case list:
      new (&value.as_list) JList(std::move(other.value.as_list));
      break;
    }
    other.cleanup();
  }

public:
  BasicJSON() : type(null), value{0} {}
  // To convert to a boolean you need to pass an extra int to differentiate
  // between bools and numbers .. use JBool method to create a boolean
  BasicJSON(bool val, int) : type(boolean), value{val, 0} {}
  BasicJSON(long double val) : type(number), value{val} {}
  BasicJSON(String val) : type(text), value{val} {}
  BasicJSON(const char *val) : type(text), value{val} {}
  BasicJSON(JMap val) : type(map), value{val} {}
  BasicJSON(JList val) : type(list), value{val} {}
  BasicJSON(const JSON &other) : type(null) { copyFromOther(other); }
  BasicJSON(JSON &&other) noexcept : type(null) {
    moveFromOther(std::move(other));
  };
  ~BasicJSON() { cleanup(); }
  Type whatIs() const { return type; }
  BasicJSON &operator=(const JSON &other) {
    copyFromOther(other);
    return *this;
  }
  BasicJSON &operator=(JSON &&other) {
    moveFromOther(std::move(other));
    return *this;
  }
  /// Render as number
  template <typename T> explicit operator T() const {
    assert(type == number);
    return value.as_num;
  }
  /// Return as a UTF8 encoded string
  operator String() const {
    assert(type == text);
    return value.as_string;
  }
  operator std::wstring() const {
    assert(type == text);
    std::wstring result;
    result.reserve(value.as_string.size() * 1.10); // Assume a 10% size increase
    utf82wchar(value.as_string.cbegin(), value.as_string.cend(),
               back_inserter(result));
    return result;
  }
  operator const JMap &() const {
    assert(type == map);
    return value.as_map;
  }
  operator const JList &() const {
    assert(type == list);
    return value.as_list;
  }
  operator JMap &() {
    assert(type == map);
    return value.as_map;
  }
  operator JList &() {
    assert(type == list);
    return value.as_list;
  }
  explicit operator bool() const {
    // If the type is null, the bool value is false
    if (type == null)
      return false;
    switch (type) {
    case null:
      return false;
    case boolean:
      return value.as_bool;
    case number:
      return value.as_num != 0;
    case text:
      return value.as_string.empty();
    case map:
      return value.as_map.empty();
    case list:
      return value.as_list.empty();
    }
    return type != null;
  }
  bool isNull() const { return type == null; }
  JSON &at(size_t i) {
    assert(type == list);
    return value.as_list.at(i);
  }
  const JSON &at(size_t i) const {
    assert(type == list);
    return value.as_list.at(i);
  }
  JSON &operator[](size_t i) {
    assert(type == list);
    return value.as_list[i];
  }
  const JSON &operator[](size_t i) const {
    assert(type == list);
    return value.as_list[i];
  }
  JSON &at(const String &i) {
    assert(type == map);
    return value.as_map.at(i);
  }
  const JSON &at(const String &i) const {
    assert(type == map);
    return value.as_map.at(i);
  }
  JSON &operator[](const std::string &i) {
    assert(type == map);
    return value.as_map[i];
  }
  const JSON &operator[](const std::string &i) const {
    assert(type == map);
    return value.as_map.at(i);
  }
  bool operator==(const JSON &other) const {
    if (other.type == type)
      switch (type) {
      case null:
        return true;
      case boolean:
        return value.as_bool == other.value.as_bool;
      case number:
        return value.as_num == other.value.as_num;
      case text:
        return value.as_string == other.value.as_string;
      case map:
        return value.as_map == other.value.as_map;
      case list:
        return value.as_list == other.value.as_list;
      }
    return false;
  }
};

template <typename T, typename A = std::allocator<char>>
std::ostream &operator<<(ostream &s, const BasicJMap<A> &j) {
  s << '{';
  auto entry = j.cbegin();
  auto end = j.cend();
  if (entry != end) {
    --end; // Second to last one so that we don't put a comma after the last one
    while (entry != end) {
      s << '"' << utf82json(entry->first) << R"(":)" << entry->second << ',';
      ++entry;
    }
    // The last entry in the map doesn't have a comma after it
    s << '"' << utf82json(entry->first) << R"(":)" << entry->second;
  }
  s << '}';
  return s;
}

template <typename T, typename A = std::allocator<char>>
std::ostream &operator<<(ostream &s, const BasicJList<A> &j) {
  s << '[';
  auto entry = j.cbegin();
  auto end = j.cend();
  if (entry != end) {
    --end; // Second to last one so that we don't put a comma after the last one
    while (entry != end) {
      s << *entry << ',';
      ++entry;
    }
    s << *entry;
  }
  s << ']';
  return s;
}

template <typename A = std::allocator<char>>
std::ostream &operator<<(ostream &s, const BasicJSON<A> &j) {
  using JSON = BasicJSON<A>;
  switch (j.type) {
  case JSON::null:
    s << "null";
    break;
  case JSON::boolean:
    s << (j.value.as_bool ? "true" : "false");
    break;
  case JSON::number:
    s << j.value.as_num;
    break;
  case JSON::text:
    s << '"' << utf82json(j.value.as_string) << '"';
    break;
  case JSON::map: {
    s << j.value.as_map;
    break;
  }
  case JSON::list: {
    s << j.value.as_list;
    break;
  }
  };
  return s;
}

template <typename A = std::allocator<char>> BasicJSON<A> JBool(bool val) {
  return BasicJSON<A>(val, 1);
}

template <typename T, typename A = std::allocator<char>>
std::vector<T> jsonToHomogenousList(const BasicJSON<A> &j) {
  std::vector<T> result;
  const BasicJList<A> &input = j;
  result.reserve(input.size());
  for (const BasicJSON<A> &item : input)
    result.push_back(static_cast<T>(item));
  return result;
}

template <typename T, typename A = std::allocator<char>>
std::map<String<A>, T> jsonToHomogenousMap(const BasicJSON<A> &j) {
  std::map<String<A>, T> result;
  const BasicJMap<A> &input = j;
  for (auto i = input.cbegin(); i != input.cend(); ++i)
    result.insert(make_pair(i->first, static_cast<T>(i->second)));
  return result;
}

using JSON = BasicJSON<std::allocator<char>>;
using JList = BasicJList<std::allocator<char>>;
using JMap = BasicJMap<std::allocator<char>>;
}
