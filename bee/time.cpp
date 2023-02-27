#include "time.hpp"

#include <cstdint>
#include <ctime>
#include <limits>

using std::numeric_limits;
using std::string;

namespace bee {

namespace {

constexpr int64_t sec_in_nanos = 1000000000ll;

int64_t clock_nanos(clockid_t clock)
{
  struct timespec tp;
  auto res = clock_gettime(clock, &tp);
  assert(res == 0);
  return tp.tv_nsec + tp.tv_sec * sec_in_nanos;
}

} // namespace

Time::Time() : Time(0) {}

Time Time::of_nanos_since_epoch(int64_t ts_nanos) { return Time(ts_nanos); }

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
int64_t Time::to_secs_since_epoch() const { return _ts_nanos / sec_in_nanos; }

Time Time::monotonic() { return Time(clock_nanos(CLOCK_MONOTONIC)); }

Time Time::now() { return Time(clock_nanos(CLOCK_REALTIME)); }

Span Time::diff(const Time& other) const
{
  return Span::of_nanos(_ts_nanos - other.to_nanos_since_epoch());
}

Span Time::operator-(Time other) const { return diff(other); }

Time Time::zero() { return Time(); }

string Time::to_string() const
{
  char buffer[512];
  time_t ts = to_nanos_since_epoch();
  std::strftime(buffer, sizeof(buffer), "%F %T", std::localtime(&ts));
  return buffer;
}

} // namespace bee
