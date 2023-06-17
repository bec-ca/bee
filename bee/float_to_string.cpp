#include "float_to_string.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include "fixed_rstring.hpp"
#include "int_to_string.hpp"
#include "to_string_t.hpp"

namespace bee {
namespace {

template <class T> constexpr T square(T number) { return number * number; }

namespace fast_float {

constexpr long double ce_pow10(int exp)
{
  if (exp < 0) { return 1.0 / ce_pow10(-exp); }
  if (exp == 0) { return 1.0; }
  return square(ce_pow10(exp / 2)) * (exp % 2 == 1 ? 10.0 : 1.0);
}

constexpr size_t table_size = 800;

constexpr std::array<double, table_size> make_pow10_table()
{
  std::array<double, table_size> table;
  for (size_t i = 0; i < table_size; i++) {
    table[i] = ce_pow10(i - table_size / 2);
  }
  return table;
}

constexpr auto pow10_table = make_pow10_table();

constexpr inline double pow10(int exp)
{
  return pow10_table[exp + table_size / 2];
}

} // namespace fast_float

namespace fast_int {

constexpr size_t table_size = 20;

constexpr std::array<uint64_t, table_size> make_pow10_table()
{
  std::array<uint64_t, table_size> table;
  uint64_t p = 1;
  for (size_t i = 0; i < table_size; i++) {
    table[i] = p;
    p *= 10;
  }
  return table;
}

constexpr auto pow10_table = make_pow10_table();

constexpr inline uint64_t pow10(int exp) { return pow10_table[exp]; }

constexpr inline int count_digits(auto number)
{
  int count = 1;
  if (number >= 10000000000ll) {
    number /= 10000000000ll;
    count += 10;
  }
  if (number >= 100000) {
    number /= 100000;
    count += 5;
  }
  if (number >= 1000) {
    number /= 1000;
    count += 3;
  }
  if (number >= 100) {
    number /= 100;
    count += 2;
  }
  if (number >= 10) {
    number /= 10;
    count += 1;
  }
  return count;
}

} // namespace fast_int

constexpr uint64_t max_uint = std::numeric_limits<uint64_t>::max();

struct float10 {
  constexpr float10(uint64_t mantissa, int exp) : _mantissa(mantissa), _exp(exp)
  {}

  constexpr static float10 of_float(double number)
  {
    constexpr double b = 1ull << 53;
    int exp2;
    auto mantissa2 = std::frexp(number, &exp2) * b;
    exp2 -= 53;
    auto f10 = float10(mantissa2, 0);
    while (exp2 > 0) {
      f10 = f10.mul2();
      exp2--;
    }
    while (exp2 < 0) {
      f10 = f10.div2();
      exp2++;
    }
    f10.round_inplace(16);
    return f10;
  }

  constexpr double to_float() const
  {
    return _mantissa * fast_float::pow10(_exp);
  }

  constexpr float10 mul2() const
  {
    if (_mantissa <= max_uint / 2) {
      return {_mantissa * 2, _exp};
    } else {
      return {_mantissa / 5, _exp + 1};
    }
  }

  constexpr float10 div2() const
  {
    if (_mantissa <= max_uint / 5) {
      return {_mantissa * 5, _exp - 1};
    } else {
      return {_mantissa / 2, _exp};
    }
  }

  constexpr static float10 add(float10 n1, float10 n2)
  {
    int exp = std::max(n1._exp, n2._exp);
    while (n1._exp < exp) {
      n1._mantissa /= 10;
      n1._exp++;
    }
    while (n2._exp < exp) {
      n2._mantissa /= 10;
      n2._exp++;
    }
    if (n1._mantissa <= max_uint - n2._mantissa) {
      return {n1._mantissa + n2._mantissa, exp};
    } else {
      return {n1._mantissa / 10 + n2._mantissa / 10, exp + 1};
    }
  }

  constexpr float10 operator+(const float10& other) const
  {
    return add(*this, other);
  }

  constexpr uint64_t mantissa() const { return _mantissa; };
  constexpr int exp() const { return _exp; };

  constexpr void normalize()
  {
    if (_mantissa == 0) {
      _exp = 0;
    } else {
      while (_mantissa > 0 && _mantissa % 10 == 0) { remove_digit(); }
    }
  }

  constexpr int remove_digit()
  {
    int d = _mantissa % 10;
    _mantissa /= 10;
    _exp++;
    return d;
  }

  constexpr bool is_zero() const { return _mantissa == 0; }

  constexpr inline int mantissa_num_digits() const
  {
    return fast_int::count_digits(_mantissa);
  }

  constexpr inline void mul_exp10(int exp) { _exp += exp; }

  constexpr inline void round_inplace(int digits)
  {
    int remove_digits = mantissa_num_digits() - digits;
    int last_rem = 0;
    for (int i = 0; i < remove_digits; i++) { last_rem = remove_digit(); }
    if (last_rem >= 5) { _mantissa++; }
    normalize();
  }

 private:
  uint64_t _mantissa = 0;
  int _exp = 0;
};

inline void add_digits(
  fixed_rstring<1024>& output,
  uint64_t number,
  int extra_trailing_zeroes,
  int pad_left_zeros,
  bool comma)
{
  int digits = 0;
  while (number > 0 || extra_trailing_zeroes > 0 || digits == 0 ||
         pad_left_zeros > digits) {
    if (comma && digits > 0 && digits % 3 == 0) { output.prepend(','); }
    if (extra_trailing_zeroes > 0) {
      output.prepend('0');
      extra_trailing_zeroes--;
    } else if (number > 0) {
      output.prepend((number % 10) + '0');
      number /= 10;
    } else {
      output.prepend('0');
    }
    digits++;
  }
}

inline bool should_use_exp_notation(int decimal_places, const float10& num)
{
  int max_exp = std::max(3, decimal_places);
  const int digits = num.mantissa_num_digits() + num.exp();
  return digits < -max_exp + 1 || digits > max_exp;
}

std::string format_float(double number, const FormatParams& p)
{
  if (std::isnan(number)) { return "nan"; }

  if (std::isinf(number)) {
    if (number < 0) {
      return "-inf";
    } else if (p.sign) {
      return "+inf";
    } else {
      return "inf";
    }
  }

  fixed_rstring<1024> output;

  if (number == 0) {
    if (p.exact_decimal_places) {
      for (int i = 0; i < p.decimal_places; i++) { output.prepend('0'); }
      output.prepend('.');
    }
    output.prepend('0');
    if (p.sign) { output.prepend('+'); }
    return output.to_string();
  }

  bool negative = false;
  if (number < 0) {
    negative = true;
    number = -number;
  }

  auto num = float10::of_float(number);

  if (!p.fixed && should_use_exp_notation(p.decimal_places, num)) {
    num.round_inplace(p.decimal_places + 1);
    const int want_exp = -num.mantissa_num_digits() + 1;
    int exp_n = num.exp() - want_exp;
    num.mul_exp10(-exp_n);

    bool negated = false;
    if (exp_n < 0) {
      exp_n = -exp_n;
      negated = true;
    }
    add_digits(output, exp_n, 0, 0, false);
    if (negated) { output.prepend('-'); }
    output.prepend('e');
  } else {
    num.round_inplace(num.mantissa_num_digits() + num.exp() + p.decimal_places);
  }

  if (num.exp() >= 0) {
    if (p.exact_decimal_places && p.decimal_places > 0) {
      add_digits(output, 0, p.decimal_places, 0, false);
      output.prepend('.');
    }
    add_digits(output, num.mantissa(), num.exp(), 0, p.comma);
  } else if (num.exp() < -18) {
    add_digits(output, num.mantissa(), 0, -num.exp(), false);
    output.prepend('.');
    output.prepend('0');
  } else {
    const uint64_t m = fast_int::pow10(-num.exp());
    auto d = num.mantissa() % m;
    add_digits(
      output,
      d,
      p.exact_decimal_places ? p.decimal_places - fast_int::count_digits(d) : 0,
      -num.exp(),
      false);
    output.prepend('.');
    add_digits(output, num.mantissa() / m, 0, 0, p.comma);
  }

  if (negative) {
    output.prepend('-');
  } else if (p.sign) {
    output.prepend('+');
  }

  return output.to_string();
}

} // namespace

std::string to_string_t<float>::convert(float value, const FormatParams& p)
{
  return format_float(value, p);
}

std::string to_string_t<double>::convert(double value, const FormatParams& p)
{
  return format_float(value, p);
}

} // namespace bee
