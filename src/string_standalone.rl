/** Ragel file for parsing a string in json **/

#include <string>

#ifdef DEBUG
#include <iostream>
#endif

namespace yajp {

// data //////////////////////////////////////////
%%{
    machine string;
    include "string.rl";
    main := string@gotString;
    write data;
}%%

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

    %%write init;
    %%write exec;
}

}
