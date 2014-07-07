# JSON++11

A JSON reader+writer library for C++11.

 * Easy to use
 * Super fast
 * Full automatic transparent unicode support through utf8

This is a the best c++11 JSON library that I know of. It does generation and parsing. It's all headers.

# To use in your project:

Download these headers into your source:

 * https://raw.githubusercontent.com/matiu2/json--11/master/src/json.hpp
 * https://github.com/matiu2/json--11/raw/master/src/mapper.hpp
 * https://github.com/matiu2/json--11/raw/master/src/parser.hpp

# Only one file to include

    #include "json.hpp"

# To generate json

    using namespace json;

    JSON j{
        JMap{
            {"name", "Mister Awesome"},
            {"age", 40}
            {"items", JList{
                {"Hammer"}, {20.2}, {"Sword"}
            }},
            {"This is a null value", {}}
        }
    };

    std::cout << j;

Will output something like:

    {"name":"Mister Awesome","age":40,"items":["Hammer",20.2,"Sword"],"This is a null value":null}

----

# Parsing

## Easy parsing, using a stream

    JSON myJSON;
    std::cin >> myJSON;

# Conversion

Most conversion is done through casting.

## Examples

 * [unicode](examples/unicode.cpp Unicode)

    // Cast to int
    JSON j{20};
    int output{j};
    cout << output << " = " << (int)j << endl; 
    // output: 20 = 20

    // Cast to string
    JSON j{"This is a \\u16E5 string"};
    std::string output{j};
    // output now contains 


# Advanced parsing

There several 'json::read' funcitions, for stream, string and other iterators:

    JSON read(std::istream &in, bool skipOverErrors = false);
    JSON read(const std::string &in, bool skipOverErrors = false);
    template<typename T>
    JSON read(T begin, T end, bool skipOverErrors = false) {

If your input contains several JSON values, use these functions. They will return the point where they finished parsing, so you can get the next bit.

    using StreamIterator=std::istream_iterator<char>;
    using LocStreamIterator = LocatingIterator<StreamIterator>;
    std::pair<JSON, LocStreamIterator> readWithPos(std::istream &in, bool skipOverErrors = false);

    using LocStringIterator = LocatingIterator<std::string::const_iterator>;
    std::pair<JSON, LocStringIterator> readWithPos(const std::string &in, bool skipOverErrors = false);

    template<typename T>
    std::pair<JSON, LocatingIterator<T>>  readWithPos(T begin, T end, bool skipOverErrors = false);
