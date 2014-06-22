/** Converts UTF8 encoded strings into JSON strings **/
#pragma once

#include <string>

#include <stdexcept>
#include <cassert>
#include <string>
#include <iostream>
#include <iterator>

using namespace std;

/// Takes a number, and outputs its hexadecimal representation
template <typename T, typename OutIterator>
void hex(T number, OutIterator& out) {
    int shift = sizeof(T)*8; // 4 bits for one hex digit
    const T mask = 0xf;
    bool stillZero = true;

    while (shift > 0) {
        shift -= 4;
        T work = (number >> shift) & mask;
        if (stillZero) {
            // Don't output leading zeros
            if (work == 0) {
                continue;
            } else {
                stillZero = false;
            }
        }
        if (work >= 0xA)
            *out++ = 'A' - 0xA + work; 
        else *out++ = '0' + work;
    }
}

/// Handles a UTF8 char that's encoded in a multibyte format
template <typename Iterator>
wchar_t handleMultiByteChar(Iterator& in, const Iterator& end) {
    // Check the number of bytes
    unsigned char mask = 0xFC; 
    int digitsToGo = 6; 
    wchar_t result = 0;

    assert(*in & 0xC0); // Must have top two bits set to use this func or all will fail

    /* Using the chart here: https://en.wikipedia.org/wiki/UTF-8#Description
     * ===========================================================================================================
     * Bits of    | First      | Last Code  | Byte 1      | Byte 2   | Byte 3   | Byte 4   | Byte 5   | Byte 6   |
     * code point | Code Point | Point      |             |          |          |          |          |          |
     * ===========================================================================================================
     * 7          | U+0000     | U+007F     | 1  0xxxxxxx |          |          |          |          |          |
     * 11         | U+0080     | U+07FF     | 2  110xxxxx | 10xxxxxx |          |          |          |          |
     * 16         | U+0800     | U+FFFF     | 3  1110xxxx | 10xxxxxx | 10xxxxxx |          |          |          |
     * 21         | U+10000    | U+1FFFFF   | 4  11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |          |          |
     * 26         | U+200000   | U+3FFFFFF  | 5  111110xx | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |          |
     * 31         | U+4000000  | U+7FFFFFFF | 6  1111110x | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
     * 
     * If the 1st byte matches the 110x xxxx pattern (0xC0) .. we know there is one more byte and so on
     **/

    // Work out how many digits to go by shifting the mask left until we find a match
    while ((*in & mask) != mask) {
        --digitsToGo;
        mask <<= 1;
    }

    // Now use the mask to read the value
    result = *in++ & (char)~mask;

    mask = 0x3f; // 0011 1111
    while (--digitsToGo && (in != end)) {
        result = (result << 6) | (*in++ & mask);
    }

    if (digitsToGo)
        throw std::logic_error("ran out of bytes while reading a multibyte char");

    return result;
}


/// Converts a UTF8 into JSON string
template <typename Iterator, typename OutIterator>
void utf82json(Iterator in, Iterator end, OutIterator out) {
    while (in != end) {
        unsigned char c = *in;
        // Handle UTF8 continuations first
        if ((c & 0xC0) == 0xC0) { // If top 2 bits are set (1100 0000)
            wchar_t mb = handleMultiByteChar(in, end);
            *out++ = '\\';
            *out++ = 'u';
            hex(mb, out);
            continue;
        }
        // Handle quoted chars
        switch (c) {
            case '\b': // 0x08
                *out++ = '\\';
                *out++ = 'b';
                break;
            case '\t': // 0x09
                *out++ = '\\';
                *out++ = 't';
                break;
            case '\n': // 0x0A
                *out++ = '\\';
                *out++ = 'n';
                break;
            case '\f': // 0x0C
                *out++ = '\\';
                *out++ = 'f';
                break;
            case '\r': // 0xOD
                *out++ = '\\';
                *out++ = 'r';
                break;
            case '"': // 0x22
                *out++ = '\\';
                *out++ = '"';
                break;
            case '\\': // 0x5C
                *out++ = '\\';
                *out++ = '\\';
                break;
            case '/': // 0x2F
                *out++ = '\\';
                *out++ = '/';
                break;
            default:
                *out++ = *in;
        }
        ++in; // Get the next char
    }
}

template <typename Iterator, typename OutIterator>
void utf82wchar(Iterator in, Iterator end, OutIterator out) {
    while (in != end) {
        if ((*in & 0xC0) == 0xC0) { // If top 2 bits are set (1100 0000)
            wchar_t mb = handleMultiByteChar(in, end);
            *out = mb;
        } else {
            // Just copy the single byte chars
            *out = *in;
        }
        ++out;
        ++in; // Get the next char
    }
}

/** Converts a UTF8 enconded string into a json string
 */
std::string utf82json(const std::string& utf8) {
    std::string result;
    result.reserve(utf8.size() * 1.1); // Assume a 10% size increase
    utf82json(utf8.cbegin(), utf8.cend(), back_inserter(result));
    return result;
}

/** Converts a UTF8 enconded string into a json string
 */
void utf82json(const std::string& utf8, std::string& out) {
    utf82json(utf8.cbegin(), utf8.cend(), back_inserter(out));
}

