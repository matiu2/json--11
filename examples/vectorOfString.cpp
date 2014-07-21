#include "../src/json.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace json;
using namespace std;

int main(int, char **) {
  // Parsing
  std::string input = R"(["Eggs", "Bacon", "Beans"])";
  JSON json = read(input);
  vector<std::string> output = jsonToHomogenousList<std::string>(json);
  cout << "JSON: " << input << endl
       << "read: " << json << endl
       << "output: ";
  for (const auto &s : output)
    cout << s << ", ";
  cout << endl;
}

