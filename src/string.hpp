
#line 1 "/home/matiu/projects/json++11/src/string_standalone.rl"
/** Ragel file for parsing a string in json **/

#include <string>

#ifdef DEBUG
#include <iostream>
#endif

namespace yajp {

// data //////////////////////////////////////////

#line 16 "/home/matiu/projects/json++11/src/string.hpp"
static const int string_start = 1;
static const int string_first_final = 5;
static const int string_error = 0;

static const int string_en_main = 1;


#line 17 "/home/matiu/projects/json++11/src/string_standalone.rl"


/**
* @brief Parse a json string
*
* @tparam T any class that has  a 'foundString' method that we can call
* @param json the json that we are parsing .. should be already at the start of the string
* @param mapper an object that we can call 'foundString(const std::string&) or foundString(std::string&&) (c++11 move) on
* @param expectedSize the expected size of the output string
*/
template <class T>
void parseString(const std::string& json, T& mapper, unsigned long expectedSize=256) {
    // Ragel vars
    int cs;
    const char *p = &json.c_str()[0];
    const char *pe = p + json.length();
    // action vars
    unsigned long uniChar = 0;
    std::string currentString;
    currentString.reserve(expectedSize);

    
#line 47 "/home/matiu/projects/json++11/src/string.hpp"
	{
	cs = string_start;
	}

#line 39 "/home/matiu/projects/json++11/src/string_standalone.rl"
    
#line 54 "/home/matiu/projects/json++11/src/string.hpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr0:
#line 9 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = *p; }
	goto st1;
tr3:
#line 4 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = '\b'; }
	goto st1;
tr4:
#line 5 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = '\f'; }
	goto st1;
tr5:
#line 6 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = '\n'; }
	goto st1;
tr6:
#line 7 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = '\r'; }
	goto st1;
tr7:
#line 8 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = '\t'; }
	goto st1;
tr11:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            *output++ = uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                *output++ = *i;
        }
    }
#line 9 "/home/matiu/projects/json++11/src/string.rl"
	{ *output++ = *p; }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 130 "/home/matiu/projects/json++11/src/string.hpp"
	switch( (*p) ) {
		case 34: goto tr1;
		case 92: goto st2;
	}
	goto tr0;
tr1:
#line 63 "/home/matiu/projects/json++11/src/string.rl"
	{
        ++p;
        return done();
    }
	goto st5;
tr12:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            *output++ = uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                *output++ = *i;
        }
    }
#line 63 "/home/matiu/projects/json++11/src/string.rl"
	{
        ++p;
        return done();
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 192 "/home/matiu/projects/json++11/src/string.hpp"
	goto st0;
st0:
cs = 0;
	goto _out;
tr13:
#line 26 "/home/matiu/projects/json++11/src/string.rl"
	{
        /*
           UCS-4 range (hex.)           UTF-8 octet sequence (binary)
           0000 0000-0000 007F   0xxxxxxx
           0000 0080-0000 07FF   110xxxxx 10xxxxxx
           0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

           0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
           0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
           0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
        */
        int numBytes = getNumBytes(uniChar); // number of bytes needed for utf-8 encoding
        if (numBytes == 1) {
            *output++ = uniChar;
        } else {
            std::vector<char> bytes;
            for (int i=1; i<numBytes; ++i) {
                char byte = uniChar & 0x3f; // Only encoding 6 bits right now
                byte |= 0x80; // Make sure the high bit is set
                bytes.push_back(byte);
                uniChar >>= 6;
            }
            // The last byte is special
            char mask = 0x3f >> (numBytes - 2);
            char byte = uniChar & mask;
            char top = 0xc0;
            for (int i=2; i<numBytes; ++i) {
                top >>= 1;
                top |= 0x80;
            }
            byte |= top;
            bytes.push_back(byte);
            // Output it
            for (auto i=bytes.rbegin(); i!=bytes.rend(); ++i)
                *output++ = *i;
        }
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 241 "/home/matiu/projects/json++11/src/string.hpp"
	switch( (*p) ) {
		case 98: goto tr3;
		case 102: goto tr4;
		case 110: goto tr5;
		case 114: goto tr6;
		case 116: goto tr7;
		case 117: goto tr8;
	}
	goto tr0;
tr8:
#line 10 "/home/matiu/projects/json++11/src/string.rl"
	{
        uniChar = 0;
        uniCharBytes = 0;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 262 "/home/matiu/projects/json++11/src/string.hpp"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr9;
	} else
		goto tr9;
	goto st0;
tr9:
#line 14 "/home/matiu/projects/json++11/src/string.rl"
	{
        if (uniCharBytes++ == 4)
            throw std::logic_error("Max unicode char is 32 bits");
        uniChar <<= 4;
        char ch = *p;
        if (ch >= 'a') 
            uniChar += ch - 'a' + 0x0A;
        else if (ch >= 'A')
            uniChar += ch - 'A' + 0x0A;
        else
            uniChar += ch - '0';
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 291 "/home/matiu/projects/json++11/src/string.hpp"
	switch( (*p) ) {
		case 34: goto tr12;
		case 92: goto tr13;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr9;
	} else
		goto tr9;
	goto tr11;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 40 "/home/matiu/projects/json++11/src/string_standalone.rl"
}

}
