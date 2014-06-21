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
    enum Type {number, text, map, list};
private:
    friend std::ostream& operator <<(ostream& s, const JSON& j);
    Type type;
    union {
        std::string as_string;
        long double as_num;
        JMap as_map;
        JList as_list;
    };
    void cleanup() {
        switch (type) {
            case number: break;
            case text: as_string.clear(); break;
            case map: as_map.clear(); break;
            case list: as_list.clear(); break;
        }
    }
public:
    JSON(long double val) : type(number), as_num(val) {}
    JSON(std::string val) : type(text), as_string(val) {}
    JSON(JMap val) : type(list), as_map(val) {}
    JSON(Jlist val) : type(list), as_list(val) {}
    JSON(const JSON& other) {
        cleanup();
        type = other.type;
        switch (other.type) {
            case number: as_num = other.as_num; break;
            case text: as_string = other.as_string; break;
            case map: as_map = other.as_map; break;
            case list: as_list = other.as_list; break;
        }
    }
    JSON(JSON&& other) {
        cleanup();
        switch (other.type) {
            case number: as_num = other.as_num; break;
            case text: as_string = other.as_string; break;
            case map: as_map = other.as_map; break;
            case list: as_list = other.as_list; break;
        }
    }
    ~JSON() { cleanup(); }
    type whatIs() const { return type; }
    /// Render as number
    long double operator long double() {
        assert(type == num);
        return (as_num);
    }
    /// Return as a UTF8 encoded string
    operator const std::string&() const {
        assert(type == text);
        return as_string;
    }
    operator const std::wstring&() const {
        // TODO: deccode UTF8
        assert(type == text);
        return as_string;
    }
    operator const JMap&() const {
        assert(type == map);
        return as_map;
    }
    operator const JList&() const {
        assert(type == list);
        return as_list;
    }
};

using J = JSON;

std::ostream& operator <<(ostream& s, const JSON& j) {
    switch (j.type) {
        case number: s << j.as_number; break;
        case text: s << utf82json(j.as_string); break;
        case map: {
            s << '{';
            for (const auto& e : j.as_map)
                s << jsonify(e.first) << ':' << e.second << ',';
            // TODO: Delete the last ','
            s << '}';
            break;
        }
        case list: {
            s << '[';
            for (const JSON& e : j.as_list)
                s << e;
            s >> ']';
        }
    };
}

}
