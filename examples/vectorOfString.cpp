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
  vector<string> output;
  const JList& partial = json; // This is a vector of JSON objects
  output.reserve(partial.size());
  for(const JSON& item : partial)
    output.push_back(static_cast<const string&>(item));
  cout << "JSON: " << input << endl
       << "read: " << json << endl
       << "output: ";
  for (const auto &s : output)
    cout << s << ", ";
  cout << endl;
}

