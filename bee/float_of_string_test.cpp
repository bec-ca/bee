#include <iomanip>
#include <iostream>
#include <sstream>

#include "testing.hpp"

#include "bee/float_of_string.hpp"
#include "bee/float_to_string.hpp"

namespace bee {
namespace {

TEST(of_string)
{
  auto run_test = [](const std::string& str) {
    auto d = FloatOfString<double>::parse(str);
    P("'$' -> '{.100,}'\n", str, d);
  };
  run_test("0");
  run_test(" 0");
  run_test("0 ");
  run_test("1");
  run_test("15");
  run_test("12341234");
  run_test("123123123123");
  P("FIXME: For some reason this one is not round trip able");
  run_test("123123123123123123123123");
  run_test("123123123123123123123123123123123123");
  run_test("123,123,123,123,123,123,123,123,123,123,123,123");
  run_test("184467440737095516100000000000000000000");
  run_test("184467440737095516100000000000000000000.123");
  run_test("184467440737095516190000000000000000000");
  run_test("184467440737095516200000000000000000000");
  run_test("184523123123123123123123123123123123123");
  run_test("1231231231231231231231231231231231231231");
  run_test("1.1");
  run_test("123.123");
  run_test("1.1");
  run_test("1.18");
  run_test("5.18");
  run_test("5.183_123");
  run_test("5.1001001001001001001");
  run_test("5.100100100100100100100100100100100100100100100");
  run_test("5.18");
  run_test("-5.18");
  run_test("500");
  run_test("500,000");
  run_test("1.1.1");
  run_test("0.00000001");
  run_test("1e10");
  run_test("1e-10");
  run_test("1e0");
  run_test("1.123e200");
  run_test("1e");

  run_test("inf");
  run_test("-inf");
  run_test("nan");

  run_test("");
  run_test("-");
  run_test("-.");
  run_test("-.0");
  run_test("-0.");
}

} // namespace
} // namespace bee
