/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>

#include "json.hpp"

using namespace bandit;
using namespace json;

go_bandit([](){
    describe("Basic JSON", [](){
        it("1.1. Can be null", [](){
            JSON j;
            AssertThat(j.isNull(), Equals(true));
            AssertThat(j.whatIs(), Equals(JSON::null));
        });
        it("1.2. Can be true", [](){
            JSON j = JBool(true);
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::boolean));
            AssertThat(j, Equals(true));
        });
        it("1.3. Can be false", [](){
            JSON j = JBool(false);
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::boolean));
            AssertThat(j, Equals(false));
        });
        it("1.4. Can be an int", [](){
            JSON j(2);
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::number));
            AssertThat((int)j, Equals(2));
        });
        it("1.5. Can be a double", [](){
            JSON j(3.1415927);
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::number));
            AssertThat((long double)j, Equals(3.1415927));
        });
        it("1.6. Can be a string", [](){
            JSON j("Hello there");
            AssertThat(j.isNull(), Equals(false));
            AssertThat(j.whatIs(), Equals(JSON::text));
            AssertThat((const std::string&)j, Equals("Hello there"));
        });
    });
});

int main(int argc, char* argv[])
{
  return bandit::run(argc, argv);
}
