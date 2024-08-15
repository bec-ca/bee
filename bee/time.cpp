#include "time.hpp"

#include <cstdint>
#include <limits>

#include "date.hpp"
#include "parse_string.hpp"
#include "string_util.hpp"

using std::numeric_limits;
using std::string;

namespace bee {

namespace {

constexpr int64_t second_in_nanos = 1000000000ll;
constexpr int64_t minute_in_nanos = 60 * second_in_nanos;
constexpr int64_t hour_in_nanos = 60 * minute_in_nanos;
constexpr int64_t day_in_nanos = 24 * hour_in_nanos;

const Date unix_epoch_date(1970, 1, 1);

const Error invalid_time_format_error("Invalid time format");

int64_t clock_nanos(clockid_t clock)
{
  struct timespec tp;
  auto res = clock_gettime(clock, &tp);
  assert(res == 0);
  return tp.tv_nsec + tp.tv_sec * second_in_nanos;
}

} // namespace

Time::Time() : Time(0) {}

Time Time::of_nanos_since_epoch(int64_t ts_nanos) { return Time(ts_nanos); }

Time Time::of_span_since_epoch(const Span& span)
{
  return Time(span.to_nanos());
}

Time Time::of_date(const Date& date)
{
  return Time() + Span::of_days(date - unix_epoch_date);
}

Time Time::epoch() { return Time(0); }

bool Time::operator==(Time other) const { return _ts_nanos == other._ts_nanos; }
bool Time::operator!=(Time other) const { return _ts_nanos != other._ts_nanos; }
bool Time::operator<(Time other) const { return _ts_nanos < other._ts_nanos; }
bool Time::operator<=(Time other) const { return _ts_nanos <= other._ts_nanos; }
bool Time::operator>(Time other) const { return _ts_nanos > other._ts_nanos; }
bool Time::operator>=(Time other) const { return _ts_nanos >= other._ts_nanos; }

Time::Time(int64_t ts_nanos) : _ts_nanos(ts_nanos) {}

Time Time::min()
{
  return Time::of_nanos_since_epoch(numeric_limits<int64_t>::min());
}

Time Time::max()
{
  return Time::of_nanos_since_epoch(numeric_limits<int64_t>::max());
}

Time Time::operator+(Span span) const
{
  return Time::of_nanos_since_epoch(_ts_nanos + span.to_nanos());
}

Time& Time::operator+=(Span span)
{
  _ts_nanos += span.to_nanos();
  return *this;
}

int64_t Time::to_nanos_since_epoch() const { return _ts_nanos; }
int64_t Time::to_secs_since_epoch() const
{
  return _ts_nanos / second_in_nanos;
}

Time Time::monotonic() { return Time(clock_nanos(CLOCK_MONOTONIC)); }

Time Time::now() { return Time(clock_nanos(CLOCK_REALTIME)); }

Span Time::diff(const Time& other) const
{
  return Span::of_nanos(_ts_nanos - other.to_nanos_since_epoch());
}

Span Time::operator-(Time other) const { return diff(other); }

Time Time::zero() { return Time(); }

OrError<Time> Time::of_string(const std::string& str)
{
  auto parts = split_space(str, 2);
  if (parts.size() != 2) { return invalid_time_format_error; }
  bail(date, Date::of_string(parts[0]));
  auto time_parts = split(parts[1], ":");
  if (time_parts.size() != 3) { return invalid_time_format_error; }
  bail(h, parse_string<int>(time_parts[0]));
  bail(m, parse_string<int>(time_parts[1]));
  bail(s, parse_string<double>(time_parts[2]));
  return of_date(date) + Span::of_seconds(m * 60 + h * 60 * 60 + s);
}

string Time::_time_of_day() const
{
  auto ts = to_nanos_since_epoch();
  ts %= day_in_nanos;

  int hours = ts / hour_in_nanos;
  ts %= hour_in_nanos;

  int minutes = ts / minute_in_nanos;
  ts %= minute_in_nanos;

  double seconds = ts / double(second_in_nanos);

  return F("{02}:{02}:{02.9}", hours, minutes, seconds);
}

string Time::_date() const
{
  return (unix_epoch_date + to_nanos_since_epoch() / day_in_nanos).to_string();
}

string Time::to_string() const { return F("$ $", _date(), _time_of_day()); }

string Time::to_string_filename() const
{
  return F("$_$", _date(), _time_of_day());
}

} // namespace bee
