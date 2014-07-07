#include "../src/json.hpp"

#include <iostream>
#include <string>

using namespace json;
using namespace std;

int main(int, char**)
{
   std::string input{R"("This is a \u16E5 string")"};
   JSON j = read(input);
   std::string output = static_cast<std::string&>(j);
   cout << "JSON: " << j << endl
        << "result: " << output << endl;
}
