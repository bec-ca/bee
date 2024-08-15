#include "date.hpp"

#include <stdexcept>

#include "parse_string.hpp"
#include "string_util.hpp"

namespace bee {
namespace {

constexpr int month_to_days[] = {
  0,
  31,
  28,
  31,
  30,
  31,
  30,
  31,
  31,
  30,
  31,
  30,
  31,
};

constexpr int get_days_in_regular_year()
{
  int out = 0;
  for (int d : month_to_days) { out += d; }
  return out;
}

constexpr int days_in_regular_year = get_days_in_regular_year();
constexpr int days_in_leap_year = days_in_regular_year + 1;

const Error invalid_date_format_error("Invalid date format");

constexpr bool is_leap_year(int year)
{
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

constexpr int get_days_in_month(int month, int year)
{
  if (month == 2) {
    if (is_leap_year(year)) {
      return 29;
    } else {
      return 28;
    }
  } else {
    return month_to_days[month];
  }
}

constexpr int get_days_in_year(int year)
{
  return (is_leap_year(year) ? days_in_leap_year : days_in_regular_year);
}

constexpr int get_date_index_of_the_first_day_of_year(int year)
{
  year--;
  return year * days_in_regular_year + year / 4 - year / 100 + year / 400;
}

constexpr int get_year_of_date_index(int date_index)
{
  int year = 1;
  for (;;) {
    int remaining = date_index - get_date_index_of_the_first_day_of_year(year);
    int days_in_this_year = get_days_in_year(year);
    if (remaining <= days_in_this_year) { break; }
    year += std::max(1, remaining / days_in_leap_year);
  }
  return year;
}

OrError<int> make_date_index(const DateTriple& triple)
{
  if (triple.year <= 0) { shot("year must be >= 1, got $", triple.year); }
  if (triple.month <= 0 || triple.month > 12) {
    shot("month must be >= 1 && <= 12, got $", triple.month);
  }
  int days_in_month = get_days_in_month(triple.month, triple.year);
  if (triple.day <= 0 || triple.day > days_in_month) {
    shot("day must be >= 1 && <= $, got $", days_in_month, triple.day);
  }
  int index = get_date_index_of_the_first_day_of_year(triple.year);
  for (int i = 1; i < triple.month; i++) {
    index += get_days_in_month(i, triple.year);
  }
  index += triple.day - 1;
  return index;
}

} // namespace

std::string DateTriple::to_string() const
{
  return F("{04}-{02}-{02}", year, month, day);
}

Date::Date() : _date_index(0) {}
Date::Date(int date) : _date_index(date) {}

Date::Date(int year, int month, int day) : Date(DateTriple{year, month, day}) {}

Date::Date(const DateTriple& triple)
    : _date_index(make_date_index(triple).value())
{}

int Date::operator-(const Date& other) const
{
  return _date_index - other._date_index;
}

Date Date::operator+(int days) const { return Date(_date_index + days); }
Date Date::operator-(int days) const { return Date(_date_index - days); }

Date& Date::operator++()
{
  _date_index++;
  return *this;
}

Date Date::operator++(int)
{
  auto ret = *this;
  _date_index++;
  return ret;
}

Date& Date::operator--()
{
  _date_index--;
  return *this;
}

Date Date::operator--(int)
{
  auto ret = *this;
  _date_index--;
  return ret;
}

Date& Date::operator+=(int days)
{
  _date_index += days;
  return *this;
}

Date& Date::operator-=(int days)
{
  _date_index -= days;
  return *this;
}

DateTriple Date::to_triple() const
{
  int year = get_year_of_date_index(_date_index);
  int month = 1;
  int day = _date_index - get_date_index_of_the_first_day_of_year(year) + 1;

  for (;;) {
    int days_in_month = get_days_in_month(month, year);
    if (day <= days_in_month) { break; }
    day -= days_in_month;
    month++;
    if (month == 13) {
      month = 1;
      year++;
    }
  }
  return {year, month, day};
}

std::string Date::to_string() const { return to_triple().to_string(); }

OrError<Date> Date::make_date(int year, int month, int day)
{
  return make_date({year, month, day});
}

OrError<Date> Date::make_date(const DateTriple& triple)
{
  bail(date_index, make_date_index(triple));
  return Date(date_index);
}

OrError<Date> Date::of_string(const std::string& str)
{
  auto parts = split(str, "-");
  if (parts.size() != 3) { return invalid_date_format_error; }
  bail(year, parse_string<int>(parts[0]));
  bail(month, parse_string<int>(parts[1]));
  bail(day, parse_string<int>(parts[2]));
  return make_date({year, month, day});
}

} // namespace bee
