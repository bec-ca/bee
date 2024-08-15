#include <limits>

#include "int_of_string.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(int)
{
  auto f = IntOfString<int>::parse;
  PRINT_EXPR(f(""));
  PRINT_EXPR(f("a"));
  PRINT_EXPR(f("-"));
  PRINT_EXPR(f("123"));
  PRINT_EXPR(f("123x"));
  PRINT_EXPR(f(" 123"));
  PRINT_EXPR(f("123 "));
  PRINT_EXPR(f("-123"));
  PRINT_EXPR(f("+123"));
  PRINT_EXPR(f("2000000000"));
  PRINT_EXPR(f("4000000000"));

  PRINT_EXPR(f("2147483647"));
  PRINT_EXPR(f("2147483648"));
  PRINT_EXPR(f("21474830000"));

  PRINT_EXPR(f("-2147483647"));
  PRINT_EXPR(f("-2147483648"));
  PRINT_EXPR(f("-2147483649"));
}

TEST(long_long)
{
  auto f = IntOfString<long long>::parse;
  PRINT_EXPR(f(""));
  PRINT_EXPR(f("a"));
  PRINT_EXPR(f("-"));
  PRINT_EXPR(f("123"));
  PRINT_EXPR(f("123x"));
  PRINT_EXPR(f(" 123"));
  PRINT_EXPR(f("123 "));
  PRINT_EXPR(f("-123"));
  PRINT_EXPR(f("+123"));
  PRINT_EXPR(f("2000000000"));
  PRINT_EXPR(f("4000000000"));
  PRINT_EXPR(f("8000000000000000000"));

  PRINT_EXPR(f("9223372036854775807"));
  PRINT_EXPR(f("9223372036854775808"));
  PRINT_EXPR(f("92233720368547758000"));

  PRINT_EXPR(f("-9223372036854775807"));
  PRINT_EXPR(f("-9223372036854775808"));
  PRINT_EXPR(f("-9223372036854775809"));
}

TEST(unsigned_long_long)
{
  auto f = IntOfString<unsigned long long>::parse;
  PRINT_EXPR(f("-123"));
  PRINT_EXPR(f("+123"));
  PRINT_EXPR(f("2000000000"));
  PRINT_EXPR(f("4000000000"));
  PRINT_EXPR(f("8000000000000000000"));

  PRINT_EXPR(f("9223372036854775808"));
  PRINT_EXPR(f("18446744073709551615"));
  PRINT_EXPR(f("18446744073709551616"));
  PRINT_EXPR(f("184467440737095516000"));
}

} // namespace
} // namespace bee
