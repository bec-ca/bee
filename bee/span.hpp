#pragma once

#include "error.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace bee {

struct Span {
 public:
  static Span of_nanos(int64_t nanos);
  static Span of_micros(double micros);
  static Span of_millis(double millis);
  static Span of_seconds(double seconds);
  static Span of_minutes(double seconds);
  static Span of_hours(double seconds);

  int64_t to_nanos() const;
  int64_t to_micros() const;
  int64_t to_millis() const;
  int64_t to_seconds() const;

  double to_float_micros() const;
  double to_float_millis() const;
  double to_float_seconds() const;

  static const Span min;
  static const Span max;

  bool operator==(const Span& other) const;
  bool operator!=(const Span& other) const;
  bool operator<(const Span& other) const;
  bool operator<=(const Span& other) const;
  bool operator>(const Span& other) const;
  bool operator>=(const Span& other) const;

  bool is_zero() const;

  Span operator-(const Span& other) const;
  Span operator+(const Span& other) const;

  Span& operator+=(const Span& other);
  Span& operator-=(const Span& other);

  Span operator/(int64_t by) const;
  Span operator*(int64_t by) const;

  static Span zero();

  std::string to_string() const;

  void sleep() const;

  std::chrono::nanoseconds to_chrono() const;

 private:
  explicit Span(int64_t span);
  int64_t _span_nanos;
};

} // namespace bee
