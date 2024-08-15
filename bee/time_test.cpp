#include "testing.hpp"
#include "time.hpp"

namespace bee {
namespace {

TEST(to_string)
{
  PRINT_EXPR(Time());
  PRINT_EXPR((Time() + Span::of_hours(400000)));
}

TEST(of_date)
{
  auto r = [](int y, int m, int d) { return Time::of_date(Date(y, m, d)); };
  PRINT_EXPR(r(2023, 01, 30));
  PRINT_EXPR(r(1970, 01, 01));
  PRINT_EXPR(r(2000, 12, 31));
  PRINT_EXPR(r(2000, 02, 29));
  PRINT_EXPR(r(2000, 03, 01));
  PRINT_EXPR(r(2001, 03, 01));

  PRINT_EXPR(r(2001, 03, 01) + Span::of_hours(23.3412));
}

TEST(of_string)
{
  PRINT_EXPR(Time::of_string(""));
  PRINT_EXPR(Time::of_string("2000-01-01"));
  PRINT_EXPR(Time::of_string("2000-01-01 12"));
  PRINT_EXPR(Time::of_string("2000-01-01 12:02:03.56"));
  PRINT_EXPR(Time::of_string("2000-01-01 12:02:03.123456789"));

  PRINT_EXPR(Time::of_string("2023-09-20 23:58:43"));
}

} // namespace
} // namespace bee
