/// Parses a json string
#pragma once

#include <iterator>

#include "utils.hpp"
#include "string_utils.hpp"
#include "utf8encode.hpp"

namespace json {

/// When we come across a block of normal chars, return the char just past the
/// end of it
template <typename Iterator>
inline void findEndOfUnchangedCharBlock(Iterator p, Iterator pe) {
  while (p != pe) {
    switch (*p) {
    case '\\':
    case '"':
      return;
    default:
      ++p;
    };
  }
}

/// Most basic string parser. Calls back functions for each token/block that it
/// finds.
template <typename Iterator>
inline void
parseString(Iterator p, Iterator pe,
            std::function<void(Iterator, Iterator)>
                recordUnchangedChars, /// Takes the begin and end iterators
                                      /// of a block of unchanged chars
            std::function<void(char)> recordChar,
            std::function<void(wchar_t)> recordUnicode) {
  /// Handle the 4 digits of a unicode char
  auto readUnicode = [&]() {
    wchar_t u = 0;
    int uniCharNibbles = 0;
    while (p != pe) {
      if (uniCharNibbles == sizeof(wchar_t))
        throw std::logic_error("Max unicode char is 32 bits");
      u <<= 4;
      char ch = *p++;
      if ((ch >= 'a') && (ch <= 'f'))
        u += ch - 'a' + 0x0A;
      else if ((ch >= 'A') && (ch <= 'F'))
        u += ch - 'A' + 0x0A;
      else if ((ch >= '0') && (ch <= '9'))
        u += ch - '0';
      else
        break;
      ++uniCharNibbles;
    }
    // We didn't get any unicode digits
    if (uniCharNibbles == 0) {
      throw std::logic_error("\\u with no hex after it");
    } else {
      return u;
    }
  };
  /// Handle the escaped character after the backslash
  /// @return true if we handled it; false if it turned out to be just an normal
  /// char (eg '\\')
  auto handleEscape = [&]() {
    switch (*++p) {
    case 'b':
      recordChar('\b');
      break;
    case 'f':
      recordChar('\f');
      break;
    case 'n':
      recordChar('\n');
      break;
    case 'r':
      recordChar('\r');
      break;
    case 't':
      recordChar('\t');
      break;
    case 'u':
      recordUnicode(readUnicode());
      break;
    default:
      return false;
    };
    return true;
  };

  // Main outter string parsing loop
  Iterator unchangedCharsStart = p;
  while (p != pe) {
    switch (*p) {
    case '"':
      return;
    case '\\':
      if (handleEscape())
        break;
    // If the escaped char was just a normal char, it will be the start of a
    // chain of normal chars, so don't break out of the switch statement here;
    // continue on to handle the chain
    default: {
      unchangedCharsStart = p;
      findEndOfUnchangedCharBlock(p, pe);
      recordUnchangedChars(unchangedCharsStart, p);
    }
    }
  }
}

/// The most efficient string parser. Overwrites the input. Requires an assignable value_type
/// Returns the end of the re-written string
template <typename Iterator>
inline enable_if<is_copy_assignable<remove_pointer<Iterator>>(), string_reference<Iterator>> parseString(Iterator p, Iterator pe) {
  string_reference<Iterator> result{p, p};
  bool hadChangedChars = false;

  // Just increments the output string's length
  auto recordUnchangedChars = [&](Iterator begin, Iterator end) {
    if (!hadChangedChars) {
      // If we haven't had any changed chars, just increment our output position
      if (is_random_access_iterator<Iterator>())
        result.end += begin - end;
      else
        while (begin++ != end)
          ++result.end;
    } else {
      // Overwrite the output. All JSON converstions are shorter than the raw json.
      // So the output will easily be contained within the original json string's size
      result.end = std::copy(begin, end, result.end);
    }
  };

  auto recordChar = [&](char c) {
    *(result.end++) = c;
  };
  
  auto recordUnicode = [&](wchar_t u) {
    // UTF-8 encode the char
    result.end = utf8encode(u, result.end);
  };

  parseString<Iterator>(p, pe, recordUnchangedChars, recordChar, recordUnicode);

  return result;
}


}
