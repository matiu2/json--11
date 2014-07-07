#include "../src/json.hpp"

#include <iostream>
#include <map>
#include <string>

using namespace json;
using namespace std;

int main(int, char **) {
  // Parsing
  std::string input = R"({"Eggs": 2, "Bacon": 2000, "Beans": 12})";
  JSON json = read(input);
  map<string, int> output = jsonToHomogenousMap<int>(json);
  cout << "JSON: " << input << endl
       << "read: " << json << endl
       << "output: ";
  for (const auto &pair : output)
    cout << pair.first << ": " << pair.second << ", ";
  cout << endl;
}

