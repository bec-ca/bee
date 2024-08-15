#include "float_of_string.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

#include "bee/int_of_string.hpp"

namespace bee {
namespace {

const Error invalid_float_format_error("Malformed number");

template <class T> OrError<T> of_string_impl(const std::string_view& str)
{
  if (str == "inf") {
    return std::numeric_limits<T>::infinity();
  } else if (str == "-inf") {
    return -std::numeric_limits<T>::infinity();
  } else if (str == "nan") {
    if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
      return std::numeric_limits<T>::quiet_NaN();
    } else {
      std::numeric_limits<T>::signaling_NaN();
    }
  }

  if (str.empty()) { return invalid_float_format_error; }

  size_t idx = 0;
  bool negated = false;
  const char c = str[idx];
  if (c == '-') {
    negated = true;
    idx++;
  } else if (c == '+') {
    idx++;
  }

  T value = 0;
  bool is_decimals = false;
  bool has_digits = false;
  int exponent = 0;
  for (; idx < str.size(); idx++) {
    char c = str[idx];
    if (isdigit(c)) {
      int d = c - '0';
      value = value * 10.0 + d;
      if (is_decimals) { exponent--; }
      has_digits = true;
    } else if (c == ',' || c == '_') {
      continue;
    } else if (c == '.') {
      if (is_decimals) { return invalid_float_format_error; }
      is_decimals = true;
    } else if (c == 'e') {
      bail(e, IntOfString<int>::parse(str.substr(idx + 1)));
      exponent += e;
      break;
    } else {
      return invalid_float_format_error;
    }
  }

  if (!has_digits) { return invalid_float_format_error; }

  if (negated) { value = -value; }
  if (exponent != 0) { value *= std::pow(10.0, exponent); }
  return value;
}

} // namespace

#define IMPLEMENT_PARSER(T, F)                                                 \
  OrError<T> FloatOfString<T>::parse(const std::string_view& str)              \
  {                                                                            \
    return of_string_impl<T>(str);                                             \
  }

IMPLEMENT_PARSER(float, std::stof);
IMPLEMENT_PARSER(double, std::stod);

} // namespace bee
