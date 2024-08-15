#pragma once

#include <cstdint>
#include <string>

#include "date.hpp"
#include "error.hpp"
#include "span.hpp"

namespace bee {

struct Time {
 public:
  static Time of_nanos_since_epoch(int64_t ts_nanos);
  static Time of_span_since_epoch(const Span& span);
  static Time of_date(const Date& date);
  static Time epoch();

  Time();

  static Time min();
  static Time max();

  bool operator==(Time other) const;
  bool operator!=(Time other) const;
  bool operator<(Time other) const;
  bool operator<=(Time other) const;
  bool operator>(Time other) const;
  bool operator>=(Time other) const;

  Time& operator+=(Span span);
  Time operator+(Span span) const;

  Span operator-(Time other) const;

  int64_t to_nanos_since_epoch() const;
  int64_t to_secs_since_epoch() const;

  static Time monotonic();
  static Time now();

  Span diff(const Time& other) const;

  static Time zero();

  static OrError<Time> of_string(const std::string& str);
  std::string to_string() const;
  std::string to_string_filename() const;

 private:
  explicit Time(int64_t ts);

  std::string _time_of_day() const;
  std::string _date() const;

  int64_t _ts_nanos;
};

} // namespace bee
