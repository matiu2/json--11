/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>
#include <sstream>

#include "json.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("JSON Generation", [&]() {

    std::stringstream output;
    before_each([&]() { output.str(""); });

    it("1.1. Can be null", [&]() {
      JSON j;
      AssertThat(j.isNull(), Equals(true));
      AssertThat(j.whatIs(), Equals(JSON::null));
      output << j;
      AssertThat(output.str(), Equals("null"));
    });

    it("1.2. Can be true", [&]() {
      JSON j = JBool(true);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(true));
      output << j;
      AssertThat(output.str(), Equals("true"));
    });

    it("1.3. Can be false", [&]() {
      JSON j = JBool(false);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(false));
      output << j;
      AssertThat(output.str(), Equals("false"));
    });

    it("1.4. Can be an int", [&]() {
      JSON j(2);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(2));
      output << j;
      AssertThat(output.str(), Equals("2"));
    });

    it("1.5. Can be a double", [&]() {
      JSON j(3.1415927);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Equals(3.1415927));
      output << j;
      AssertThat(output.str(), Equals("3.14159"));
    });

    it("1.6. Can be a string", [&]() {
      JSON j("Hello there");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals("Hello there"));
      output << j;
      AssertThat(output.str(), Equals("\"Hello there\""));
    });

    it("1.7. Reads in UTF8", [&]() {
      constexpr const char *s = u8"ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ";
      JSON j(s);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals(s));
      output << j;
      AssertThat(output.str(),
                 Equals(R"("\u16A0\u16C7\u16BB\u16EB\u16D2\u16E6\u16A6\u16EB)"
                        R"(\u16A0\u16B1\u16A9\u16A0\u16A2\u16B1\u16EB\u16A0)"
                        R"(\u16C1\u16B1\u16AA\u16EB\u16B7\u16D6\u16BB\u16B9)"
                        R"(\u16E6\u16DA\u16B3\u16A2\u16D7")"));
    });

    it("1.8. Reads in a list", [&]() {
      JSON j(JList{{}, {"String"}, {4.0}});
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::list));
      const JList &list = j;
      AssertThat(list, HasLength(3));
      output << j;
      AssertThat(output.str(), Equals("[null,\"String\",4]"));
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
      const JMap &map = j;
      AssertThat(map.size(), Equals((size_t)2));
      output << j;
      AssertThat(
          output.str(),
          Equals(R"({"Lot )"
                 R"(1":{"make":"hillman","model":"Hunter","year":1974},"Lot )"
                 R"(2":{"make":"porsche","model":"Cayenne","year":1982}})"));
    });
  });

  describe("JSON Parsing", [&]() {

    std::stringstream output;

    before_each([&]() {
      output.str("");
    });

    it("2.1. Parses null", [&]() {
      JSON j = read("null");
      AssertThat(j.isNull(), Equals(true));
      AssertThat(j.whatIs(), Equals(JSON::null));
      output << j;
      AssertThat(output.str(), Equals("null"));
    });

    it("2.2. Parses true", [&]() {
      JSON j = read("true");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(true));
      output << j;
      AssertThat(output.str(), Equals("true"));
    });

    it("2.3. Parses false", [&]() {
      JSON j = read("false");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(false));
      output << j;
      AssertThat(output.str(), Equals("false"));
    });

    it("2.4. Parses a simple int", [&]() {
      JSON j = read("5");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(5));
      output << j;
      AssertThat(output.str(), Equals("5"));
    });

    it("2.5. Parses a negative int", [&]() {
      JSON j = read("-15");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(-15));
      output << j;
      AssertThat(output.str(), Equals("-15"));
    });

    it("2.6. Parses a negative double", [&]() {
      JSON j = read("-12345.67");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Is().EqualToWithDelta(-12345.67, 0.01));
      output.precision(7);
      output << j;
      AssertThat(output.str(), Equals("-12345.67"));
    });

    it("2.7. Parses an exponent", [&]() {
      JSON j = read("-1234567e-2");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Is().EqualToWithDelta(-12345.67, 0.01));
      output.precision(7);
      output << j;
      AssertThat(output.str(), Equals("-12345.67"));
    });

    it("2.8. Parses a utf8 string", [&]() {
      std::string input(
          R"("Hello \u05D0\u05E0\u05D9 \u05D9\u05DB\u05D5\u05DC )"
          R"(\u05DC\u05D0\u05DB\u05D5\u05DC )"
          R"(\u05D6\u05DB\u05D5\u05DB\u05D9\u05EA \u05D5\u05D6\u05D4 )"
          R"(\u05DC\u05D0 \u05DE\u05D6\u05D9\u05E7 \u05DC\u05D9")");
      std::string decoded("Hello אני יכול לאכול זכוכית וזה לא מזיק לי");
      JSON j = read(input);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals(decoded));
      // It should output exactly as it read
      output << j;
      AssertThat(output.str(), Equals(input));
    });


  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
