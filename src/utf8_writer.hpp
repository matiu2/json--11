#pragma once
/// Funtions to convert wchar_t to a utf8 encoded character sequence

namespace json {

/**
 * Returns the number of bytes needed to encode a unicode character in utf8
 */
int getNumBytes(wchar_t c) {
  int result = 1;
  wchar_t checker = 0x7f;
  if (c <= checker)
    return result;
  checker <<= 4;
  checker |= 0xf;
  result += 1;
  if (c <= checker)
    return result;
  while (c > checker) {
    checker <<= 5;
    checker |= 0x1f;
    ++result;
  }
  return result;
}

/// After collecting all the unicode integers, we generate the final output char
template <typename T> void endUnicode(T &p) {
  /*
     UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     0000 0000-0000 007F   0xxxxxxx
     0000 0080-0000 07FF   110xxxxx 10xxxxxx
     0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

     0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
  */
  int numBytes =
      getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
  if (numBytes == 1) {
    output += uniChar;
  } else {
    std::vector<char> bytes;
    for (int i = 1; i < numBytes; ++i) {
      char byte = uniChar & 0x3f; // Only encoding 6 bits right now
      byte |= 0x80;               // Make sure the high bit is set
      bytes.push_back(byte);
      uniChar >>= 6;
    }
    // The last byte is special
    char mask = 0x3f >> (numBytes - 2);
    char byte = uniChar & mask;
    char top = 0xc0;
    for (int i = 2; i < numBytes; ++i) {
      top >>= 1;
      top |= 0x80;
    }
    byte |= top;
    bytes.push_back(byte);
    // Output it
    for (auto i = bytes.rbegin(); i != bytes.rend(); ++i)
      output += *i;
  }
}
}