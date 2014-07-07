/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>
#include <sstream>

#include "json.hpp"
#include "test_input.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  before_each([&]() { output.str(""); });

  describe("The JSON model", [&]() {

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

  });

  describe("The JSON model as a map", [&]() {

    JSON input(JMap{
        {"Lot 1",
         JMap{{"make", {"hillman"}}, {"model", {"Hunter"}}, {"year", {1974}}}},
        {"Lot 2", JMap{{"make", {"porsche"}},
                       {"model", {"Cayenne"}},
                       {"year", {1982}}}}});

    it("2.9. Reads in a map", [&]() {
      JSON j(input);
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

    it("2.10. Has map access", [&]() {
      JSON j(input);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::map));
      JSON &lot1 = j.at("Lot 1");
      AssertThat(lot1.whatIs(), Equals(JSON::map));
      JSON &jyear = lot1.at("year");
      // TODO: make a test that calls .at("Something that doesn't exist");
      AssertThat(jyear.whatIs(), Equals(JSON::number));
      int year(lot1["year"]);
      AssertThat(year, Equals(1974));
      AssertThat(static_cast<int>(j["Lot 1"]["year"]), Equals(1974));
    });

    it("2.11. Throws when it can't find a map element", [&]() {
      JSON j(input);
      AssertThrows(std::out_of_range, j.at("Something that doesn't exist"));
    });

    it("2.12. Allows adding map entries using [] operators", [&]() {
      JSON j(input);
      j["Lot 3"] = {"Not here"};
      std::string lot3 = j.at("Lot 3");
      AssertThat(lot3, Equals("Not here"));
    });

    it("2.13. Allows altering map entries using [] operators", [&]() {
      JSON j(input);
      j["Lot 1"]["year"] = {1960};
      int year{j["Lot 1"]["year"]};
      AssertThat(year, Equals(1960));
    });

  });

  describe("The model as a list", [&]() {

    JSON input{JList{1, 2, 3, 4, 5}};

    it("3.1. Reads in a list", [&]() {
      std::vector<int> value;
      JList& l(input);
      value.reserve(l.size());
      for (JSON& j : l)
        value.push_back((int)j);
      AssertThat(value, Equals(std::vector<int>{1, 2, 3, 4, 5}));
    });

    it("3.2. Allows at() access to a list", [&]() {
      int x{input.at(2)};
      AssertThat(x, Equals(3));
    });

    it("3.3. Allows [] assignment to a list", [&]() {
      input[2] = 999;
      AssertThat((int)input.at(2), Equals(999));
    });

    it("3.4. at(999) throws out_of_range", [&]() {
      AssertThrows(std::out_of_range, input.at(9999));
    });
  });

  describe("JSON Parsing", [&]() {

    it("4.1. Parses null", [&]() {
      JSON j = read("null");
      AssertThat(j.isNull(), Equals(true));
      AssertThat(j.whatIs(), Equals(JSON::null));
      output << j;
      AssertThat(output.str(), Equals("null"));
    });

    it("4.2. Parses true", [&]() {
      JSON j = read("true");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(true));
      output << j;
      AssertThat(output.str(), Equals("true"));
    });

    it("4.3. Parses false", [&]() {
      JSON j = read("false");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(false));
      output << j;
      AssertThat(output.str(), Equals("false"));
    });

    it("4.4. Parses a simple int", [&]() {
      JSON j = read("5");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(5));
      output << j;
      AssertThat(output.str(), Equals("5"));
    });

    it("4.5. Parses a negative int", [&]() {
      JSON j = read("-15");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(-15));
      output << j;
      AssertThat(output.str(), Equals("-15"));
    });

    it("4.6. Parses a negative double", [&]() {
      JSON j = read("-12345.67");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Is().EqualToWithDelta(-12345.67, 0.01));
      output.precision(7);
      output << j;
      AssertThat(output.str(), Equals("-12345.67"));
    });

    it("4.7. Parses an exponent", [&]() {
      JSON j = read("-1234567e-2");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Is().EqualToWithDelta(-12345.67, 0.01));
      output.precision(7);
      output << j;
      AssertThat(output.str(), Equals("-12345.67"));
    });

    it("4.8. Parses a utf8 string", [&]() {
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

    it("4.9. Parses a list", [&]() {
      JSON j = read(R"xxx(["a", 1, 2.4, ["another", "list"], "12"])xxx");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::list));
      const JList &l = j;
      auto i = l.cbegin();
      const JSON &a = *i;
      // Examine the elements
      AssertThat(a.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)*i, Equals("a"));
      AssertThat((int)*++i, Equals(1));
      AssertThat((long double)*++i, EqualsWithDelta(2.4, 0.01));
      // Examine the inner list
      const JList &l2 = *++i;
      auto i2 = l2.cbegin();
      AssertThat((const std::string &)*i2++, Equals("another"));
      AssertThat((const std::string &)*i2++, Equals("list"));
      AssertThat(i2, Equals(l2.cend()));
      // The last element is a string, not a number
      AssertThat((++i)->whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)*i, Equals("12"));
      // Print out the JSON again
      output << j;
      AssertThat(output.str(),
                 Equals(R"xxx(["a",1,2.4,["another","list"],"12"])xxx"));
    });

    it("4.10. Parses a map", [&]() {
      std::string json(R"xxx({"Name": "Human", "Weapons": ["Teeth", "Bony )xxx"
                       R"xxx(Forehead", "Hands", "Feet"], "Traits": )xxx"
                       R"xxx({"Strength": 4, "Intelligence": 2}})xxx");
      JSON j = read(json);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::map));
      const JMap &data = j;
      const JMap::mapped_type& name = data.at("Name");
      // Name: Human
      AssertThat((const std::string &)name, Equals("Human"));
      // Weapons: ["Teeth", "Bony Forehead", "Hands", "Feet"]
      const auto& weapons = data.at("Weapons");
      std::vector<std::string> expectedWeapons{"Teeth", "Bony Forehead", "Hands", "Feet"};
      const JList& actualWeapons(weapons);
      AssertThat(actualWeapons, EqualsContainer(expectedWeapons));
      // Traits
      const JMap& traits = data.at("Traits");
      AssertThat((int)traits.at("Strength"), Equals(4));
      AssertThat((int)traits.at("Intelligence"), Equals(2));
    });

    it("4.11. Handle a missing comma in a list", [&]() {
      std::string json(R"XXX([1, 2, 3 4])XXX");
      using Error = JSONParser<std::string::const_iterator>::Error;
      AssertThrows(Error, read(json));
#ifndef NO_LOCATIONS
      AssertThat(
          LastException<Error>().what(),
          Equals(
              "Expected a ',' or a ']' but hit a '4' instead at row 1 col 10"));
      AssertThat(LastException<Error>().row, Equals(1));
      AssertThat(LastException<Error>().col, Equals(10));
#endif
    });

    it("4.12. Handle a non closed list", [&]() {
      std::string json(R"XXX([1, 2, 3)XXX");
      using Error = JSONParser<std::string::const_iterator>::Error;
      AssertThrows(Error, read(json));
#ifndef NO_LOCATIONS
      AssertThat(
          LastException<Error>().what(),
          Equals(
              "Expected a ',' or a ']' but hit the end of the input at row 1 col 9"));
      AssertThat(LastException<Error>().row, Equals(1));
      AssertThat(LastException<Error>().col, Equals(9));
#endif
    });

    it("4.13. Handle 2 decimal points", [&]() {
      // Note: I'm not sure what the correct behaviour should be with this. As
      // it is it reads 1.2, then hits the '.' and says, EOI because '.'s not
      // part of number
      std::string json(R"XXX(1.2.3.4)XXX");
      JSON j = read(json);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, EqualsWithDelta(1.2, 0.01));
      output << j;
      AssertThat(output.str(), Equals("1.2"));
    });

    it("4.14. bad map", [&]() {
      std::string json(R"XXX({)XXX");
      using Error = JSONParser<std::string::const_iterator>::Error;
      AssertThrows(Error, read(json));
#ifndef NO_LOCATIONS
      AssertThat(LastException<Error>().what(),
                 Equals(
                     R"(hit end while looking for '"' to signify the start of )"
                     R"(an attribute value at row 1 col 2)"));
      AssertThat(LastException<Error>().row, Equals(1));
      AssertThat(LastException<Error>().col, Equals(2));
#endif
    });

    it("4.15. Just plain bad", [&]() {
      std::string json(R"XXX(aoeu)XXX");
      using Error = JSONParser<std::string::const_iterator>::Error;
      AssertThrows(Error, read(json));
#ifndef NO_LOCATIONS
      AssertThat(LastException<Error>().what(),
                 Equals(
                     R"(Couldn't Identify next JSON Type at row 1 col 1)"));
      AssertThat(LastException<Error>().row, Equals(1));
      AssertThat(LastException<Error>().col, Equals(1));
#endif
    });

    it("4.16. Can parse from a stream, and has map access", [&]() {
        std::stringstream input;
        input << realJson;
        JSON json;
        input >> json;
        const std::string& token = json["access"]["token"]["id"];
        AssertThat(token, Equals("abcdxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));
    });
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
