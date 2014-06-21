/* Ragel file for parsing json.

** History **

Originally I tried to do it all in one go. However, that's not possible as JSON is recursive.

I then tried to do it as 'in one go' as possible, thinking to make it the fastest json parser available, but that's not very flexible to program on top of.

** Summary **

I've turned a lot of the ragel state machines into hard coded switch based state machines. The only two left that are ragel 'string' and 'number'.

*/

#pragma once
#ifndef YAJP_JSON_HPP
#define YAJP_JSON_HPP

#include <string>
#include <stdexcept>
#include <functional>
#include <vector>
#include <sstream>
#include <map>

namespace yajp {

%%{
    # Harder types

    # string machine
    machine string;
    include string "string.rl";
    main := string;

    # Number machine
    machine number;
    include "number.rl";
    main := number;

}%%

/// Allows lazy evaluation of number types
class JSONNumberInfo {
private:
    bool _intIsNeg;
    unsigned long long _intPart;
    int _expPart;
public:
    JSONNumberInfo(bool intIsNeg, unsigned long long intPart, int expPart) : _intIsNeg(intIsNeg), _intPart(intPart), _expPart(expPart) {}

    template<typename NumberType>
    NumberType value() const {
        NumberType result = _intPart;
        int expPart = _expPart;
        if (expPart < 0) {
            while (expPart++ < 0)
                result *= 0.1;
            return _intIsNeg ? -result : result;
        } else {
            while (expPart-- > 0)
                result *= 10;
            return _intIsNeg ? -result : result;
        }
    }

    operator int() { return value<int>(); }
    operator unsigned int() { return value<unsigned int>(); }
    operator long() { return value<long>(); }
    operator unsigned long() { return value<unsigned long>(); }
    operator long long() { return value<long long>(); }
    operator unsigned long long() { return value<unsigned long long>(); }
    operator float() { return value<float>(); }
    operator double() { return value<double>(); }
    operator short() { return value<short>(); }
    operator unsigned short() { return value<unsigned short>(); }
    operator char() { return value<char>(); }
    operator unsigned char() { return value<unsigned char>(); }
};

class JSONParser;

class JSONParserError : public std::runtime_error {
private:
    const JSONParser* _parser;
    const char* _location;
    std::string _msg;
    std::string make_msg(const char* location, const std::string& msg) {
        return msg + " at " + location;
    }
public:
    JSONParserError(const JSONParser* parser, const char* location, const std::string& msg) :
        std::runtime_error(make_msg(location, msg)),
        _parser(parser),
        _location(location),
        _msg(msg) { }
    const JSONParser* parser() const { return _parser; }
    const char* location() const { return _location; }
    const std::string& msg() const { return _msg; }
};

class JSONParser {
public:
    enum JSONType { null='n', boolean='t', array='[', object='{', number='0', string='"', HIT_END=0, ERROR='x' };
private:
    // Ragel vars
    const char *p;
    const char *pe;
    const char *eof;
    // Our vars
    bool skipOverErrors;
    
    /// Searches though whitespace for a '"' meaning the start of an attribute name
    void readAttributeStart() {
        while ((p < pe) && (p < eof)) {
            switch (*p++) {
            case 9:
            case 10:
            case 13:
            case ' ':
                continue;
            case '"':
                return;
            default:
                throw JSONParserError(this, p, "Couldn't find '\"' to signify the start of an attribute value");
            }
        }
        throw JSONParserError(this, p, "hit end while looking for '\"' to signify the start of an attribute value");
    }

    /**
    * If 'skipOverErrors' is false, throws a JSONParserError, otherwise just returns.
    * Context is automatically passed to the JSONParserError object.
    *
    * @param message The error message to show
    */
    void handleError(const std::string& message) {
        if (skipOverErrors) {
            // Skip Forward until we find a new type
            while (p < pe) {
                switch (getNextType(true)) {
                    case number:
                        --p; // Put the read cursor in the right position to read the number
                        return;
                    case ERROR:
                        ++p;
                        continue;
                    case HIT_END:
                        // We have to raise an error here. There's no way we can skip forward anymore
                        throw JSONParserError(this, p, std::string("Hit end: ") + message);
                    default:
                        return;
                }
            }
        } else
            throw JSONParserError(this, p, message);
    }

    /**
    * Tests a null terminated C string. If we reach the null char .. just returns.
    * If something doesn't match, it throws the JSONParserError.
    *
    * @param test A null terminated C string to compare 'p' against.
    */
    void checkStaticString(const char* test) {
        while (*test)
            if ((*test++) != (*p++))
                handleError(std::string("Static String '") + test + "' doesn't match");
    }

    /**
     * Returns the number of bytes needed to encode a unicode character in utf8
     */ 
    int getNumBytes (wchar_t c) {
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


public:
    /// @param json - KEEP THIS STRING ALIVE .. WE DONT COPY IT .. WE USE IT .. You can't just call it with an ("inplace string")
    JSONParser(const std::string& json, bool skipOverErrors=false) :
        p(json.c_str()), pe(p+json.length()), eof(pe), skipOverErrors(skipOverErrors) {}
    /// @param json - KEEP THIS STRING ALIVE .. WE DONT COPY IT .. WE USE IT .. You can't just call it with an ("inplace string")
    JSONParser(const char* json, const char* end, bool skipOverErrors=false) :
        p(json), pe(end), eof(pe), skipOverErrors(skipOverErrors) {}
    JSONParser(JSONParser&& original, bool skipOverErrors=false) :
        p(original.p), pe(original.pe), eof(original.eof), skipOverErrors(skipOverErrors) {}

    /**
    * Eats whitespace, then tells you the next type found in the JSON stream.
    * It eats the first letter for all types except number.
    *
    * @param returnError defaults to False .. true is used internally by 'handleError'
    *    returnError - skipOverErrors - behaviour on syntax error - on end of stream
    *      true      -   true         - returns ERROR             - returns HIT_END
    *      true      -   false        - returns ERROR             - returns HIT_END
    *      false     -   true         - skips forward until next type and returns that - returns HIT_END
    *      false     -   false        - throws JSONParserError    - throws JSONParserError
    *
    * @return the 'JSONType' found,
    */
    JSONType getNextType(bool returnError=false) {
        while ((p < pe) && (p < eof)) {
            switch( (*p++) ) {
                case 9: case 10: case 13:
                case ' ':
                    continue;
                case '"': return string;
                case '-': case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': {
                    --p;
                    return number;
                }
                case '[': return array;
                case 'f': return boolean;
                case 'n': return null;
                case 't': return boolean;
                case '{': return object;
                default:
                    break;
            }
        }
        if ((p >= pe) || (p >= eof)) {
            returnError = true;
        }
        if (returnError)
            return p < pe ? ERROR : HIT_END;
        else {
            handleError("Couldn't Identify next JSON Type");
            return getNextType(returnError);
        }
    }

    void readNull() { checkStaticString("ull"); }

    /**
    * Read a boolean value, either 'true' or 'false'. Assume's you've already found it with 'getNextType'
    *
    * @return The value of the boolean read.
    */
    bool readBoolean() {
        switch (*p++) {
            case 'r':
                checkStaticString("ue");
                return true;
            case 'a':
                checkStaticString("lse");
                return false;
            default:
                handleError("Couldn't read 'true' nor 'false'");
                // Once the code reaches here, it means that error raising is turned off
                // And we couldn't read a boolean value, so it has skipped us ahead to the start
                // of the next json value. We'll just keep trying to read the boolean until we get it
                // or we stack overflow.
                return readBoolean();
        }
    }

    /**
    * @brief Reads in a number from the json stream
    *
    * @tparam T the type of number we expect to find, defaults to double
    *
    * @return The value of the number we read
    */
    template <typename T=double>
    T readNumber() {
        bool intIsNeg=false; // true if the int part is negative
        bool expIsNeg=false; // true if the exponent part is negative
        unsigned long long intPart=0; // The integer part of the number
        int expPart1=0; // The inferred exponent part gotten from counting the decimal digits
        int expPart2=0; // The explicit exponent part from the number itself, added to the inferred exponent part
        bool gotAtLeastOneDigit = false;
        auto makeJSONNumber = [&expIsNeg, &expPart1, &expPart2, &intIsNeg, &intPart]() {
            long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
            return JSONNumberInfo(intIsNeg, intPart, expPart);
        };
        %%machine number;
        int startState = 
            %%write start;
        ;
        int cs = startState; // Current state
        %%{
            write exec;
        }%%
        // The state machine returns, so the code will only get here if it can't parse the string
        if (gotAtLeastOneDigit)
            return makeJSONNumber();
        else
            handleError("Couldn't read a number");
        return T();
    }

    std::string readString() {
        %%machine string;
        int startState = 
            %%write start;
        ;
        int cs = startState; // Current state
        wchar_t uniChar = 0;
        int uniCharBytes = 0;
        std::string output;
        %%{
            write exec;
        }%%
        // The state machine returns, so the code will only get here if it can't parse the string
        handleError("Couldn't read a string");
        return output;
    }

    /**
    * @brief checks if we have more array items to read
    *
    * @return true if there are more array to read; false if we've hit the end; throw's for unexpected hit
    */
    bool doIHaveMoreArray() {
        return doIHaveMore<']'>();
    }

    /**
    * @brief While reading an object .. get the next attribute name
    */
    std::string readNextAttribute() {
        readAttributeStart();
        std::string output = readString();
        return output;
    }

    /**
      * Consumes and throws away one value
      **/
    void consumeOneValue() {
        switch (getNextType()) {
            case JSONParser::null:
                readNull();
                return;
            case JSONParser::boolean:
                readBoolean();
                return;
            case JSONParser::array:
                while (doIHaveMoreArray()) {
                    consumeOneValue();
                }
                return;
            case JSONParser::object:
                while (doIHaveMoreObject()) {
                    readNextAttribute();
                    consumeOneValue();
                }
                return;
            case JSONParser::number:
                readNumber<int>();
                return;
            case JSONParser::string:
                readString();
                return;
            case JSONParser::HIT_END:
                return;
            case JSONParser::ERROR:
                return; // Code should never hit here
        };
    }

    /**
    * Reads through whitespace, return true if it hits @a separator first, false if it hits @a end first.
    * If it hits anything other than these or whitespace, it return true and backpedals one char
    *
    * @tparam end The character that means we hit the end, no more to come
    * @tparam separator The character that means we have more to come
    *
    * @return returns true if we can expect more input, false if we just hit the end
    */
    template <char end, char separator=','>
    bool doIHaveMore() {
        while ((p < pe) && (p < eof)) {
            switch (*p++) {
            case 9:
            case 10:
            case 13:
            case ' ':
                continue;
            case separator:
                return true;
            case end:
                return false;
            default:
                --p;
                return true;
            }
        }
        handleError(std::string("Expected a '") + separator + "' or a '" + end + "' but hit the end of the input");
        return false;
    }

    /**
    * @brief While reading an object .. returns true if there is more to read. Also moves us forward in the parsing.
    *
    * @return true if there is more Object to read
    */
    bool doIHaveMoreObject() {
        return doIHaveMore<'}'>();
    }

    /// Returns the pointer to the json we are parsing
    const char* json() const { return p; }

    /// This is the type you can pass to 'readObject'
    using Reader = std::function<void()>;
    using TypeReaderPair = std::pair<JSONType, Reader>;
    using ReaderMap = std::map<std::string, TypeReaderPair>;

    /// Reads an object
    /// @a readerMap - a map of attribute name to expected JSONType, reader function pair.
    /// For expample: { { "name", { JSONType::string, [&parser, &person]() { person.setName(parser.readString()); } }}}
    /// @returns - the number of attributes read
    size_t readObject(const ReaderMap& readerMap) {
        size_t attrsRead = 0;
        while (doIHaveMoreObject()) {
            std::string nextAttrName = readNextAttribute();
            // See if we expected an attribute with that name
            auto pReader = readerMap.find(nextAttrName);
            if (pReader == readerMap.end()) {
                consumeOneValue();
                continue;
            }
            // See if the type matches what we expected
            JSONType nextTokenType = getNextType();
            auto pair = pReader->second;
            auto expectedType = pair.first;
            auto reader = pair.second;
            if (nextTokenType != expectedType) {
                if (nextTokenType == JSONType::null) {
                    readNull();
                    // If we expected something and found null,
                    // We'll just pretend we didn't find it
                    // TODO: possibly make the reader() smarter and able
                    //       to be notified about finding null
                    continue;
                }
                std::stringstream errMsg;
                errMsg << "When reading attribute "
                       << nextAttrName
                       << "I expected a value of type "
                       << expectedType
                       << "but got one of type "
                       << nextTokenType;
                throw JSONParserError(this, p, errMsg.str());
            }
            reader(); // Actually read in the value to the person object
            ++attrsRead;
        }
        return attrsRead;
    }
};

} // namespace yajp

#endif // YAJP_JSON_HPP
