#include "date.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(to_string)
{
  PRINT_EXPR(Date());
  PRINT_EXPR(Date() + 1);
  PRINT_EXPR(Date() + 10);
  PRINT_EXPR(Date() + 32);
  PRINT_EXPR(Date() + 100);
  PRINT_EXPR(Date() + 364);
  PRINT_EXPR(Date() + 365);
  PRINT_EXPR(Date() + 1000);
  PRINT_EXPR(Date() + 10000);
  PRINT_EXPR(Date() + 100000);
  PRINT_EXPR(Date() + 730000);
  PRINT_EXPR(Date() + 1000000);
  PRINT_EXPR(Date() + 3000000);

  PRINT_EXPR(Date(2000, 1, 1));
  PRINT_EXPR(Date(2000, 2, 1));
  PRINT_EXPR(Date(2000, 2, 28));
  PRINT_EXPR(Date(2000, 2, 28) + 1);
  PRINT_EXPR(Date(2000, 2, 28) + 2);

  PRINT_EXPR(Date(2001, 2, 28));
  PRINT_EXPR(Date(2001, 2, 28) + 1);

  PRINT_EXPR(Date(2001, 12, 31));
  PRINT_EXPR(Date(2001, 12, 31) + 1);
}

TEST(of_string)
{
  PRINT_EXPR(Date::of_string("0001-01-01"));
  PRINT_EXPR(Date::of_string("0000-01-01"));
  PRINT_EXPR(Date::of_string("0001-00-01"));
  PRINT_EXPR(Date::of_string("0001-01-00"));

  PRINT_EXPR(Date::of_string("0001-13-01"));
  PRINT_EXPR(Date::of_string("0001-01-32"));

  PRINT_EXPR(Date::of_string("2000-02-29"));
  PRINT_EXPR(Date::of_string("2000-02-30"));
  PRINT_EXPR(Date::of_string("2023-01-01"));
  PRINT_EXPR(Date::of_string("2023-02-01"));
  PRINT_EXPR(Date::of_string("2023-02-29"));
  PRINT_EXPR(Date::of_string("9999-12-31"));

  PRINT_EXPR(Date::of_string("9999-12"));

  // TODO: this should fail
  PRINT_EXPR(Date::of_string("9999-12-31f"));

  PRINT_EXPR(Date::of_string("9999-12-f"));
  PRINT_EXPR(Date::of_string("9999-12-12-12"));
}

TEST(operator_minus)
{
  auto r = [](const std::string& d1, const std::string d2) {
    return Date::of_string(d1).value() - Date::of_string(d2).value();
  };
  PRINT_EXPR(Date() + 730119);
  PRINT_EXPR(r("2000-01-01", "0001-01-01"));
  PRINT_EXPR(r("2000-01-01", "2000-01-01"));
  PRINT_EXPR(r("2000-01-02", "2000-01-01"));
  PRINT_EXPR(r("2000-01-01", "2000-01-02"));
  PRINT_EXPR(r("2001-01-01", "2000-01-01"));
  PRINT_EXPR(r("2002-01-01", "2001-01-01"));
}

TEST(to_triple)
{
  auto t = Date(2023, 7, 6).to_triple();
  PRINT_EXPR(t.year);
  PRINT_EXPR(t.month);
  PRINT_EXPR(t.day);
}

} // namespace
} // namespace bee
