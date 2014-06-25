#include <iostream>

#include "../json.hpp"

using namespace json;
using namespace std;

int main(int argc, char**) {
  JSON j(JList{{}, {"String"}, {4.0}});
  const JList& output = j;
  cout << j << endl;
  return 0;
}
