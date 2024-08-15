#include "int_of_string.hpp"

#include <limits>
#include <type_traits>

namespace bee {
namespace {

const Error malformed_error = Error("Malformed number");
const Error overflow_error = Error("Overflow");

template <class T> struct Converter {
  using UT = std::make_unsigned_t<T>;

  template <UT max_value>
  static OrError<T> parse_gen(bool negate, int idx, const std::string_view& str)
  {
    constexpr UT max_before_mul_10 = max_value / 10 + 1;
    if (str.empty()) { return malformed_error; }
    UT result = 0;
    if (idx >= std::ssize(str)) { return malformed_error; }

    for (; idx < std::ssize(str); idx++) {
      char c = str[idx];
      if (c < '0' || c > '9') { return malformed_error; }
      int d = c - '0';
      if (result >= max_before_mul_10) { return overflow_error; }
      result *= 10;
      if (result > max_value - d) { return overflow_error; }
      result += d;
    }

    if (negate) {
      return -T(result);
    } else {
      return result;
    }
  }

  static constexpr UT max_pos = std::numeric_limits<T>::max();
  static constexpr UT max_neg = std::numeric_limits<T>::min();

  static OrError<T> parse(const std::string_view& str)
  {
    if (str.empty()) { return malformed_error; }
    if (str[0] == '-') {
      if constexpr (std::is_unsigned_v<T>) {
        return overflow_error;
      } else {
        return parse_gen<max_neg>(true, 1, str);
      }
    }

    int idx = 0;
    if (str[0] == '+') { idx++; }
    return parse_gen<max_pos>(false, idx, str);
  }
};

} // namespace

#define IMPLEMENT_PARSER(type)                                                 \
  OrError<type> IntOfString<type>::parse(const std::string_view& str)          \
  {                                                                            \
    return Converter<type>::parse(str);                                        \
  }

IMPLEMENT_PARSER(int);
IMPLEMENT_PARSER(unsigned);

IMPLEMENT_PARSER(long);
IMPLEMENT_PARSER(unsigned long);

IMPLEMENT_PARSER(long long);
IMPLEMENT_PARSER(unsigned long long);

} // namespace bee
