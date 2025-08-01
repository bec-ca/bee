#include "time.hpp"

#include <cstdint>
#include <limits>

#include "date.hpp"
#include "parse_string.hpp"
#include "string_util.hpp"

namespace bee {

namespace {

constexpr int64_t second_in_nanos = 1000000000ll;
constexpr int64_t minute_in_nanos = 60 * second_in_nanos;
constexpr int64_t hour_in_nanos = 60 * minute_in_nanos;
constexpr int64_t day_in_nanos = 24 * hour_in_nanos;

const Date unix_epoch_date(1970, 1, 1);

const Error invalid_time_format_error("Invalid time format");

int64_t clock_nanos(const clockid_t clock)
{
  struct timespec tp;
  auto res = clock_gettime(clock, &tp);
  assert(res == 0);
  return tp.tv_nsec + tp.tv_sec * second_in_nanos;
}

tm populate_tm(const Date date, const TimeOfDay tod)
{
  const auto date_triple = date.to_triple();
  tm tm;
  tm.tm_year = date_triple.year - 1900; // tm year starts from 1900
  tm.tm_mon = date_triple.month - 1;    // tm month starts from 0
  tm.tm_mday = date_triple.day;

  tm.tm_hour = tod.hour;
  tm.tm_min = tod.minute;
  tm.tm_sec = tod.second;

  tm.tm_isdst = -1; // mktime should figure dst out

  return tm;
}

Time::Decomposed make_decomposed(const tm& lt)
{
  return {
    Date::make_date(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday).value(),
    {.hour = lt.tm_hour, .minute = lt.tm_min, .second = lt.tm_sec}};
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// TimeOfDay
//

std::string TimeOfDay::to_string() const
{
  return F("{02}:{02}:{02}", hour, minute, second);
}

////////////////////////////////////////////////////////////////////////////////
// Time
//

Time::Time() : Time(0) {}

Time Time::of_nanos_since_epoch(const int64_t ts_nanos)
{
  return Time{ts_nanos};
}

Time Time::of_span_since_epoch(const Span span)
{
  return Time{span.to_nanos()};
}

Time Time::of_date(const Date date)
{
  return Time() + Span::of_int_days(date - unix_epoch_date);
}

Time Time::epoch() { return Time(0); }

Time::Time(int64_t ts_nanos) : _ts_nanos(ts_nanos) {}

Time Time::min()
{
  return Time::of_nanos_since_epoch(std::numeric_limits<int64_t>::min());
}

Time Time::max()
{
  return Time::of_nanos_since_epoch(std::numeric_limits<int64_t>::max());
}

Time Time::operator+(const Span span) const
{
  return Time::of_nanos_since_epoch(_ts_nanos + span.to_nanos());
}

Time& Time::operator+=(const Span span)
{
  _ts_nanos += span.to_nanos();
  return *this;
}

Span Time::since_epoch() const { return Span::of_nanos(_ts_nanos); }

int64_t Time::to_nanos_since_epoch() const { return _ts_nanos; }
int64_t Time::to_secs_since_epoch() const
{
  return _ts_nanos / second_in_nanos;
}

Time Time::monotonic() { return Time(clock_nanos(CLOCK_MONOTONIC)); }

Time Time::now() { return Time(clock_nanos(CLOCK_REALTIME)); }

Span Time::diff(const Time other) const
{
  return Span::of_nanos(_ts_nanos - other.to_nanos_since_epoch());
}

Span Time::operator-(const Time other) const { return diff(other); }

Time Time::zero() { return Time(); }

OrError<Time> Time::of_string(const std::string_view str)
{
  auto parts = split_space(str, 2);
  if (parts.size() != 2) { return invalid_time_format_error; }
  bail(date, Date::of_string(parts[0]));
  auto time_parts = split(parts[1], ":");
  if (time_parts.size() != 3) { return invalid_time_format_error; }
  bail(h, parse_string<int>(time_parts[0]));
  bail(m, parse_string<int>(time_parts[1]));
  bail(s, parse_string<double>(time_parts[2]));
  return of_date(date) + Span::of_float_seconds(m * 60 + h * 60 * 60 + s);
}

std::string Time::_time_of_day(const bool fixed_decimals) const
{
  auto ts = to_nanos_since_epoch();
  ts %= day_in_nanos;

  int hours = ts / hour_in_nanos;
  ts %= hour_in_nanos;

  int minutes = ts / minute_in_nanos;
  ts %= minute_in_nanos;

  double seconds = ts / double(second_in_nanos);

  if (fixed_decimals) {
    return F("{02}:{02}:{02.9p}", hours, minutes, seconds);
  } else {
    return F("{02}:{02}:{02.9}", hours, minutes, seconds);
  }
}

std::string Time::_date() const
{
  return (unix_epoch_date + to_nanos_since_epoch() / day_in_nanos).to_string();
}

std::string Time::to_string(const FormatParams& params) const
{
  return F("$ $", _date(), _time_of_day(params.exact_decimal_places));
}

std::string Time::to_string_filename() const
{
  return F("$_$", _date(), _time_of_day(true));
}

std::string Time::to_string_localtime(const FormatParams& params) const
{
  const time_t t = to_secs_since_epoch();
  tm lt;
  localtime_r(&t, &lt);
  return ((*this) + Span::of_int_seconds(lt.tm_gmtoff)).to_string(params);
}

Time Time::make_utc_time(const Date date, const TimeOfDay tod)
{
  auto tm = populate_tm(date, tod);
  const auto time_secs = ::timegm(&tm);
  return Time::of_span_since_epoch(Span::of_int_seconds(time_secs));
}

Time Time::make_local_time(const Date date, const TimeOfDay tod)
{
  auto tm = populate_tm(date, tod);
  const auto time_secs = ::timelocal(&tm);
  return Time::of_span_since_epoch(Span::of_int_seconds(time_secs));
}

std::pair<Date, TimeOfDay> Time::decompose_utc() const
{
  const time_t t = to_secs_since_epoch();
  tm lt;
  gmtime_r(&t, &lt);
  return make_decomposed(lt);
}

std::pair<Date, TimeOfDay> Time::decompose_local() const
{
  const time_t t = to_secs_since_epoch();
  tm lt;
  localtime_r(&t, &lt);
  return make_decomposed(lt);
}

} // namespace bee
