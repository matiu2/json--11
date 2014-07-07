#include "../src/json.hpp"

#include <iostream>

using namespace json;
using namespace std;

int main(int, char**)
{
    // Cast to int
    JSON j{20};
    int output{j};
    cout << output << " = " << (int)j << endl; 
    // output: 20 = 20
}
