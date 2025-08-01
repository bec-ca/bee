#include "testing.hpp"
#include "time.hpp"

namespace bee {
namespace {

TEST(to_string)
{
  PRINT_EXPR(Time());
  PRINT_EXPR(Time() + Span::of_int_hours(400000));
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

  PRINT_EXPR(r(2001, 03, 01) + Span::of_float_hours(23.3412));
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

TEST(to_string_localtime)
{
  setenv("TZ", "America/New_York", 1);
  PRINT_EXPR(Time::of_string("2000-01-01 12:02:03.123456789")
               .value()
               .to_string_localtime());
}

TEST(to_string_decimals)
{
  const auto run_test = [](const char* str) {
    P("{} -> {p}", str, Time::of_string(str));
  };
  run_test("2000-01-01 12:02:03.56");
  run_test("2000-01-01 12:02:03");
  run_test("2000-01-01 12:02:03.123456789");
}

TEST(make_time)
{
  setenv("TZ", "America/New_York", 1);

  const auto date = Date::make_date(1987, 1, 30).value();
  P(date);

  const TimeOfDay tod{.hour = 10, .minute = 30, .second = 15};

  P(Time::make_utc_time(date, tod));

  P(Time::make_local_time(date, tod));

  P(Time::make_local_time(date, tod).to_string_localtime());

  P(Time::make_local_time(
      Date::make_date(2025, 6, 27).value(),
      {.hour = 16, .minute = 03, .second = 11})
      .to_string_localtime());
}

TEST(decompose_time_utc)
{
  const auto time = Time::of_nanos_since_epoch(1000000000000000000ll);
  P(time);

  const auto& [date, tod] = time.decompose_utc();
  P("date:$ tod:$", date, tod);
}

TEST(decompose_time_local)
{
  setenv("TZ", "America/New_York", 1);

  const auto time = Time::of_nanos_since_epoch(1000000000000000000ll);
  P(time.to_string_localtime());

  const auto& [date, tod] = time.decompose_local();
  P("date:$ tod:$", date, tod);
}

} // namespace
} // namespace bee
