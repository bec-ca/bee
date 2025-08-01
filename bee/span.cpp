#include "span.hpp"

#include <chrono>
#include <limits>
#include <thread>

#include "bee/parse_string.hpp"

namespace bee {
namespace {

struct Constants {
  static constexpr int64_t nano = 1;
  static constexpr int64_t micro = 1000;
  static constexpr int64_t milli = micro * 1000;
  static constexpr int64_t second = milli * 1000;
  static constexpr int64_t minute = second * 60;
  static constexpr int64_t hour = minute * 60;
  static constexpr int64_t day = hour * 24;
};

} // namespace

Span::Span(int64_t nanos) : _span_nanos(nanos) {}

Span Span::of_nanos(int64_t nanos) { return Span(nanos * Constants::nano); }

Span Span::of_float_micros(double micros)
{
  return Span(micros * Constants::micro);
}
Span Span::of_float_millis(double millis)
{
  return Span(millis * Constants::milli);
}
Span Span::of_float_seconds(double secs)
{
  return Span(secs * Constants::second);
}
Span Span::of_float_minutes(double mins)
{
  return Span(mins * Constants::minute);
}
Span Span::of_float_hours(double hours)
{
  return Span(hours * Constants::hour);
}
Span Span::of_float_days(double days) { return Span(days * Constants::day); }

Span Span::of_int_micros(int64_t micros)
{
  return Span(micros * Constants::micro);
}
Span Span::of_int_millis(int64_t millis)
{
  return Span(millis * Constants::milli);
}
Span Span::of_int_seconds(int64_t secs)
{
  return Span(secs * Constants::second);
}
Span Span::of_int_minutes(int64_t mins)
{
  return Span(mins * Constants::minute);
}
Span Span::of_int_hours(int64_t hours) { return Span(hours * Constants::hour); }
Span Span::of_int_days(int64_t days) { return Span(days * Constants::day); }

Span Span::zero() { return Span(0); }

bool Span::is_zero() const { return _span_nanos == 0; }
bool Span::is_positive() const { return _span_nanos > 0; }
bool Span::is_negative() const { return _span_nanos < 0; }

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

Span Span::operator-() const { return Span(-_span_nanos); }

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
  auto abs_span = std::abs(_span_nanos);
  double value;
  const char* suffix;
  if (abs_span == 0) {
    return "0s";
  } else if (abs_span < Constants::micro) {
    value = to_nanos();
    suffix = "ns";
  } else if (abs_span < Constants::milli) {
    value = to_float_micros();
    suffix = "us";
  } else if (abs_span < Constants::second) {
    value = to_float_millis();
    suffix = "ms";
  } else if (abs_span < Constants::minute) {
    value = to_float_seconds();
    suffix = "s";
  } else if (abs_span < Constants::hour) {
    value = double(_span_nanos) / Constants::minute;
    suffix = "min";
  } else {
    value = double(_span_nanos) / Constants::hour;
    suffix = "h";
  }
  return bee::to_string(value, p) + suffix;
}

const Error invalid_format_error("Invalid format");
const Error invalid_suffix_error("Invalid suffix");
const Error overflow_error("Overflow");

OrError<Span> Span::of_string(const std::string_view str)
{
  int idx = 0;
  for (; idx < std::ssize(str); ++idx) {
    char c = str[idx];
    if (
      isdigit(c) || c == '.' || c == ',' || c == '-' || c == '+' || c == 'e') {
      continue;
    } else {
      break;
    }
  }

  bail(value, bee::parse_string<double>(str.substr(0, idx)));

  auto suffix = str.substr(idx);
  if (suffix == "ns") {
    return of_nanos(value);
  } else if (suffix == "us") {
    return of_float_micros(value);
  } else if (suffix == "ms") {
    return of_float_millis(value);
  } else if (suffix == "s") {
    return of_float_seconds(value);
  } else if (suffix == "min") {
    return of_float_minutes(value);
  } else if (suffix == "h") {
    return of_float_hours(value);
  } else {
    return invalid_suffix_error;
  }
}

void Span::sleep() const { std::this_thread::sleep_for(to_chrono()); }

std::chrono::nanoseconds Span::to_chrono() const
{
  return std::chrono::nanoseconds(to_nanos());
}

} // namespace bee
