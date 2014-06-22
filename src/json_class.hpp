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
    union {
        std::string as_string;
        long double as_num;
        bool as_bool;
        JMap as_map;
        JList as_list;
    };
    void cleanup() {
        switch (type) {
            case null: 
            case boolean: 
            case number: break;
            case text: as_string.clear(); break;
            case map: as_map.clear(); break;
            case list: as_list.clear(); break;
        }
    }
    void copyFromOther(const JSON& other) {
        cleanup();
        type = other.type;
        switch (other.type) {
            case null: as_num = 0; break;
            case boolean: as_bool = other.as_bool; break;
            case number: as_num = other.as_num; break;
            case text: as_string = other.as_string; break;
            case map: as_map = other.as_map; break;
            case list: as_list = other.as_list; break;
        }
    }
    void moveFromOther(JSON&& other) {
        cleanup();
        switch (other.type) {
            case null: as_num = 0; break;
            case boolean: as_bool = other.as_bool; break;
            case number: as_num = other.as_num; break;
            case text: as_string = other.as_string; break;
            case map: as_map = other.as_map; break;
            case list: as_list = other.as_list; break;
        }
        other.cleanup();
    }
public:
    JSON() : type(null), as_num(0) {}
    explicit JSON(bool val) : type(boolean), as_bool(val) {}
    JSON(long double val) : type(number), as_num(val) {}
    JSON(std::string val) : type(text), as_string(val) {}
    JSON(JMap val) : type(map), as_map(val) {}
    JSON(JList val) : type(list), as_list(val) {}
    JSON(const JSON& other) { copyFromOther(other); }
    JSON(JSON&& other) { moveFromOther(std::move(other)); };
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
    operator long double() {
        assert(type == number);
        return (as_num);
    }
    /// Return as a UTF8 encoded string
    operator const std::string&() const {
        assert(type == text);
        return as_string;
    }
    operator std::wstring() const {
        assert(type == text);
        std::wstring result;
        result.reserve(as_string.size()*1.10); // Assume a 10% size increase
        utf82wchar(as_string.cbegin(), as_string.cend(), back_inserter(result));
        return result;
    }
    operator const JMap&() const {
        assert(type == map);
        return as_map;
    }
    operator const JList&() const {
        assert(type == list);
        return as_list;
    }
    operator bool() const {
        // If the type is null, the bool value is false
        if (type == null)
            return false;
        switch (type) {
            case null: return false;
            case boolean: return as_bool;
            case number: return as_num != 0;
            case text: return as_string.empty();
            case map: return as_map.empty();
            case list: return as_list.empty();
        }
        return type != null;
    }
    bool isNull() const { return type == null; }
};

using J = JSON;

std::ostream& operator <<(ostream& s, const JSON& j) {
    switch(j.type) {
        case JSON::null: s << "null"; break;
        case JSON::boolean: s << (j.as_bool ? "true" : "false"); break;
        case JSON::number: s << j.as_num; break;
        case JSON::text: s << utf82json(j.as_string); break;
        case JSON::map: {
            s << '{';
            auto entry = j.as_map.cbegin();
            auto end = j.as_map.cend();
            --end; // Second to last one so that we don't put a comma after the last one
            while (entry != end) {
                s << utf82json(entry->first) << ':' << entry->second << ',';
                ++entry;
            }
            // The last entry in the map doesn't have a comma after it
            s << utf82json(entry->first) << ':' << entry->second;
            s << '}';
            break;
        }
        case JSON::list: {
            s << '[';
            auto entry = j.as_list.cbegin();
            auto end = j.as_list.cend();
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

}
