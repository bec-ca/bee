#pragma once

#include "or_error.hpp"

namespace bee {

struct DateTriple {
  int year;
  int month;
  int day;

  std::string to_string() const;
};

struct Date {
 public:
  Date();
  Date(int year, int month, int day);
  Date(const DateTriple& triple);

  static OrError<Date> make_date(int year, int month, int day);
  static OrError<Date> make_date(const DateTriple& triple);

  int operator-(const Date& other) const;

  Date operator+(int days) const;
  Date operator-(int days) const;

  Date& operator++();
  Date operator++(int);

  Date& operator--();
  Date operator--(int);

  Date& operator+=(int days);
  Date& operator-=(int days);

  static OrError<Date> of_string(const std::string& str);
  std::string to_string() const;

  auto operator<=>(const Date& other) const = default;

  DateTriple to_triple() const;

 private:
  Date(int date_index);
  int _date_index;
};

} // namespace bee
