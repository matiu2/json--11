/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>
#include <sstream>

#include "json.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {
  describe("JSON Generation", []() {
    std::stringstream json;
    before_each([&]() { json.str(""); });
    it("1.1. Can be null", [&]() {
      JSON j;
      AssertThat(j.isNull(), Equals(true));
      AssertThat(j.whatIs(), Equals(JSON::null));
      json << j;
      AssertThat(json.str(), Equals("null"));
    });
    it("1.2. Can be true", [&]() {
      JSON j = JBool(true);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat(j, Equals(true));
      json << j;
      AssertThat(json.str(), Equals("true"));
    });
    it("1.3. Can be false", [&]() {
      JSON j = JBool(false);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat(j, Equals(false));
      json << j;
      AssertThat(json.str(), Equals("false"));
    });
    it("1.4. Can be an int", [&]() {
      JSON j(2);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(2));
      json << j;
      AssertThat(json.str(), Equals("2"));
    });
    it("1.5. Can be a double", [&]() {
      JSON j(3.1415927);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Equals(3.1415927));
      json << j;
      AssertThat(json.str(), Equals("3.14159"));
    });
    it("1.6. Can be a string", [&]() {
      JSON j("Hello there");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string&)j, Equals("Hello there"));
      json << j;
      AssertThat(json.str(), Equals("\"Hello there\""));
    });
    it("1.7. Reads in UTF8", [&]() {
      constexpr const char* s = u8"ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ";
      JSON j(s);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string&)j, Equals(s));
      json << j;
      AssertThat(json.str(),
                 Equals(R"("\u16A0\u16C7\u16BB\u16EB\u16D2\u16E6\u16A6\u16EB)"
                        R"(\u16A0\u16B1\u16A9\u16A0\u16A2\u16B1\u16EB\u16A0)"
                        R"(\u16C1\u16B1\u16AA\u16EB\u16B7\u16D6\u16BB\u16B9)"
                        R"(\u16E6\u16DA\u16B3\u16A2\u16D7")"));
    });
    it("1.8. Reads in a list", [&]() {
      JSON j(JList{{}, {"String"}, {4.0}});
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::list));
      const JList& output = j;
      AssertThat(output, HasLength(3));
      json << j;
      AssertThat(json.str(), Equals("[null,\"String\",4]"));
    });
    it("1.9. Reads in a map", [&]() {
      JSON j(JMap{{"Lot 1", JMap{{"make", {"hillman"}},
                                 {"model", {"Hunter"}},
                                 {"year", {1974}}}},
                  {"Lot 2", JMap{{"make", {"porsche"}},
                                 {"model", {"Cayenne"}},
                                 {"year", {1982}}}}});
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::map));
      const JMap& output = j;
      AssertThat(output.size(), Equals((size_t)2));
      json << j;
      AssertThat(
          json.str(),
          Equals(R"({"Lot )"
                 R"(1":{"make":"hillman","model":"Hunter","year":1974},"Lot )"
                 R"(2":{"make":"porsche","model":"Cayenne","year":1982}})"));
    });
  });
});

int main(int argc, char* argv[]) { return bandit::run(argc, argv); }
