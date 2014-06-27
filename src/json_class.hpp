#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>

#include "utf82json.hpp"

namespace json {

struct JSON;

using JList = std::vector<JSON>;
using JMap = std::map<std::string, JSON>;

struct JSON {
public:
    enum Type {null, boolean, number, text, map, list};
private:
    friend std::ostream& operator <<(ostream& s, const JSON& j);
    Type type;
    union Value {
        std::string as_string;
        long double as_num;
        bool as_bool;
        JMap as_map;
        JList as_list;
        Value () {}
        Value (std::string val) { new (&as_string) std::string(val); }
        Value (long double val) : as_num(val) {}
        Value (bool val, int) : as_bool(val) {} // Need to pass an int to differentiate from as_num constructor
        Value (JMap val) { new (&as_map) JMap(val); }
        Value (JList val) { new (&as_list) JList(val); }
        ~Value () {}
    } value;
    void cleanup() noexcept {
        switch (type) {
            case null: 
            case boolean: 
            case number: break;
            case text: value.as_string.~string(); break;
            case map: value.as_map.~JMap(); break;
            case list: value.as_list.~JList(); break;
        }
        type = null;
    }
    void copyFromOther(const JSON& other) {
        cleanup();
        type = other.type;
        switch (other.type) {
            case null: value.as_num = 0; break;
            case boolean: value.as_bool = other.value.as_bool; break;
            case number: value.as_num = other.value.as_num; break;
            case text:
                new (&value.as_string) std::string;
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
    void moveFromOther(JSON&& other) {
        cleanup();
        type = other.type;
        switch (other.type) {
            case null: value.as_num = 0; break;
            case boolean: value.as_bool = other.value.as_bool; break;
            case number: value.as_num = other.value.as_num; break;
            case text:
                new (&value.as_string) std::string(std::move(other.value.as_string));
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
    JSON() : type(null), value{0} {}
    // To convert to a boolean you need to pass an extra int to differentiate between bools and numbers .. use JBool method to create a boolean
    JSON(bool val, int) : type(boolean), value{val, 0} {} 
    JSON(long double val) : type(number), value{val} {}
    JSON(std::string val) : type(text), value{val} {}
    JSON(JMap val) : type(map), value{val} {}
    JSON(JList val) : type(list), value{val} {}
    JSON(const JSON& other) : type(null) { copyFromOther(other); }
    JSON(JSON&& other) noexcept : type(null) { moveFromOther(std::move(other)); };
    ~JSON() { cleanup(); }
    Type whatIs() const { return type; }
    JSON& operator=(const JSON& other) {
        copyFromOther(other);
        return *this;
    }
    JSON& operator=(JSON&& other) {
        moveFromOther(std::move(other));
        return *this;
    }
    /// Render as number
    operator long double() const {
        assert(type == number);
        return value.as_num;
    }
    /// Render as an integer
    operator long long () const {
        assert(type == number);
        return value.as_num;
    }
    operator int () const {
        assert(type == number);
        return value.as_num;
    }
    /// Return as a UTF8 encoded string
    operator const std::string&() const {
        assert(type == text);
        return value.as_string;
    }
    operator std::wstring() const {
        assert(type == text);
        std::wstring result;
        result.reserve(value.as_string.size()*1.10); // Assume a 10% size increase
        utf82wchar(value.as_string.cbegin(), value.as_string.cend(), back_inserter(result));
        return result;
    }
    operator const JMap&() const {
        assert(type == map);
        return value.as_map;
    }
    operator const JList&() const {
        assert(type == list);
        return value.as_list;
    }
    operator bool() const {
        // If the type is null, the bool value is false
        if (type == null)
            return false;
        switch (type) {
            case null: return false;
            case boolean: return value.as_bool;
            case number: return value.as_num != 0;
            case text: return value.as_string.empty();
            case map: return value.as_map.empty();
            case list: return value.as_list.empty();
        }
        return type != null;
    }
    bool isNull() const { return type == null; }
};

using J = JSON;

std::ostream& operator <<(ostream& s, const JSON& j) {
    switch(j.type) {
        case JSON::null: s << "null"; break;
        case JSON::boolean: s << (j.value.as_bool ? "true" : "false"); break;
        case JSON::number: s << j.value.as_num; break;
        case JSON::text: s << '"' << utf82json(j.value.as_string) << '"'; break;
        case JSON::map: {
            s << '{';
            auto entry = j.value.as_map.cbegin();
            auto end = j.value.as_map.cend();
            --end; // Second to last one so that we don't put a comma after the last one
            while (entry != end) {
                s << '"' << utf82json(entry->first) << R"(":)" << entry->second << ',';
                ++entry;
            }
            // The last entry in the map doesn't have a comma after it
            s << '"' << utf82json(entry->first) << R"(":)" << entry->second;
            s << '}';
            break;
        }
        case JSON::list: {
            s << '[';
            auto entry = j.value.as_list.cbegin();
            auto end = j.value.as_list.cend();
            --end; // Second to last one so that we don't put a comma after the last one
            while (entry != end) {
                s << *entry << ',';
                ++entry;
            }
            s << *entry << ']';
        }
    };
    return s;
}

JSON JBool(bool val) {
    return JSON(val, 1);
}

}
