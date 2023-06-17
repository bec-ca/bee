#include "span.hpp"

#include <chrono>
#include <limits>
#include <thread>

#include "format.hpp"

#include "bee/to_string_t.hpp"

namespace bee {
namespace {

struct Constants {
  static constexpr int64_t nano = 1;
  static constexpr int64_t micro = 1000;
  static constexpr int64_t milli = 1000000;
  static constexpr int64_t second = milli * 1000;
  static constexpr int64_t minute = second * 60;
  static constexpr int64_t hour = minute * 60;
};

} // namespace

Span::Span(int64_t nanos) : _span_nanos(nanos) {}

Span Span::of_nanos(int64_t nanos) { return Span(nanos * Constants::nano); }
Span Span::of_micros(double micros) { return Span(micros * Constants::micro); }
Span Span::of_millis(double millis) { return Span(millis * Constants::milli); }
Span Span::of_seconds(double secs) { return Span(secs * Constants::second); }
Span Span::of_minutes(double mins) { return Span(mins * Constants::minute); }
Span Span::of_hours(double hours) { return Span(hours * Constants::hour); }

Span Span::zero() { return Span(0); }

bool Span::is_zero() const { return _span_nanos == 0; }

int64_t Span::to_nanos() const { return _span_nanos / Constants::nano; }
int64_t Span::to_micros() const { return _span_nanos / Constants::micro; }
int64_t Span::to_millis() const { return _span_nanos / Constants::milli; }
int64_t Span::to_seconds() const { return _span_nanos / Constants::second; }

double Span::to_float_micros() const
{
  return double(_span_nanos) / double(Constants::micro);
}

double Span::to_float_millis() const
{
  return double(_span_nanos) / double(Constants::milli);
}

double Span::to_float_seconds() const
{
  return double(_span_nanos) / double(Constants::second);
}

bool Span::operator==(const Span& other) const
{
  return _span_nanos == other._span_nanos;
}

bool Span::operator!=(const Span& other) const
{
  return _span_nanos != other._span_nanos;
}

bool Span::operator<(const Span& other) const
{
  return _span_nanos < other._span_nanos;
}

bool Span::operator<=(const Span& other) const
{
  return _span_nanos <= other._span_nanos;
}

bool Span::operator>(const Span& other) const
{
  return _span_nanos > other._span_nanos;
}

bool Span::operator>=(const Span& other) const
{
  return _span_nanos >= other._span_nanos;
}

Span Span::operator+(const Span& other) const
{
  return Span(_span_nanos + other._span_nanos);
}

Span& Span::operator+=(const Span& other)
{
  _span_nanos += other._span_nanos;
  return *this;
}

Span Span::operator-(const Span& other) const
{
  return Span(_span_nanos - other._span_nanos);
}

Span& Span::operator-=(const Span& other)
{
  _span_nanos -= other._span_nanos;
  return *this;
}

Span Span::operator/(int64_t by) const { return Span(_span_nanos / by); }

Span Span::operator*(int64_t by) const { return Span(_span_nanos * by); }

Span& Span::operator/=(int64_t by)
{
  _span_nanos /= by;
  return *this;
}

Span& Span::operator*=(int64_t by)
{
  _span_nanos *= by;
  return *this;
}

const Span Span::min = Span::of_nanos(std::numeric_limits<int64_t>::min());
const Span Span::max = Span::of_nanos(std::numeric_limits<int64_t>::max());

std::string Span::to_string(const FormatParams& p) const
{
  auto t = [&p](double v) { return ::bee::to_string(v, p); };
  if (_span_nanos < Constants::micro) {
    return F("{}ns", t(to_nanos()));
  } else if (_span_nanos < Constants::milli) {
    return F("{}us", t(to_float_micros()));
  } else if (_span_nanos < Constants::second) {
    return F("{}ms", t(to_float_millis()));
  } else if (_span_nanos < Constants::minute) {
    return F("{}s", t(to_float_seconds()));
  } else if (_span_nanos < Constants::hour) {
    return F("{}min", t(double(_span_nanos) / Constants::minute));
  } else {
    return F("{}h", t(double(_span_nanos) / Constants::hour));
  }
}

void Span::sleep() const { std::this_thread::sleep_for(to_chrono()); }

std::chrono::nanoseconds Span::to_chrono() const
{
  return std::chrono::nanoseconds(to_nanos());
}

} // namespace bee
