#pragma once

#include <cstdint>
#include <string>

#include "date.hpp"
#include "format_params.hpp"
#include "span.hpp"

namespace bee {

struct TimeOfDay {
  int hour{0};
  int minute{0};
  int second{0};

  std::string to_string() const;
};

struct Time {
 public:
  using Decomposed = std::pair<Date, TimeOfDay>;

  Time();

  static Time of_nanos_since_epoch(int64_t ts_nanos);
  static Time of_span_since_epoch(const Span span);
  static Time of_date(const Date date);
  static Time epoch();

  static Time make_utc_time(const Date date, const TimeOfDay tod);

  static Time make_local_time(const Date date, const TimeOfDay tod);

  static Time min();
  static Time max();

  auto operator<=>(const Time& other) const = default;

  Time& operator+=(Span span);
  Time operator+(Span span) const;

  Span operator-(Time other) const;

  int64_t to_nanos_since_epoch() const;
  int64_t to_secs_since_epoch() const;

  Span since_epoch() const;

  static Time monotonic();
  static Time now();

  Span diff(const Time other) const;

  static Time zero();

  static OrError<Time> of_string(std::string_view str);
  std::string to_string(const FormatParams& params = {}) const;
  std::string to_string_filename() const;
  std::string to_string_localtime(const FormatParams& params = {}) const;

  Decomposed decompose_utc() const;
  Decomposed decompose_local() const;

 private:
  explicit Time(int64_t ts);

  std::string _time_of_day(bool fixed_decimals) const;
  std::string _date() const;

  int64_t _ts_nanos;
};

} // namespace bee
