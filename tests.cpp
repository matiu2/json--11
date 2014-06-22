/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>

#include "json.hpp"

using namespace bandit;
using namespace json;

go_bandit([](){
    describe("JSON", [](){
        it("1. Can be null", [](){
            JSON j;
            AssertThat(j.isNull(), Equals(true));
            AssertThat(j.whatIs(), Equals(JSON::null));
        });
        it("2. Can be true and false", [](){
            JSON j(true);
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::boolean));
            AssertThat(j, Equals(true));
        });
    });
    
});

int main(int argc, char* argv[])
{
  return bandit::run(argc, argv);
}
