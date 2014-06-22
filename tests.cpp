/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>

#include "json.hpp"

using namespace bandit;
using namespace json;

go_bandit([](){
    describe("JSON", [](){
        it("1. Can be null", [](){
            JSON j;
            j.isNull();
            AssertThat(j.whatIs(), Equals(JSON::null));
        });
    });
    
});

int main(int argc, char* argv[])
{
  return bandit::run(argc, argv);
}
