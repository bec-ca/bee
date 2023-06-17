#include "int_to_string.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <string>

#include "fixed_rstring.hpp"
#include "to_string_t.hpp"

namespace bee {
namespace {

constexpr int table_size = 1000;
using table3_t = std::array<std::array<char, 3>, table_size>;

constexpr table3_t make_table3() noexcept
{
  table3_t out;

  for (int i = 0; i < table_size; i++) {
    for (int j = 0, n = i; j < 3; j++) {
      out[i][j] = n % 10 + '0';
      n /= 10;
    }
  }
  return out;
}

constexpr table3_t table3 = make_table3();

template <class T>
inline std::string format_int(T number_orig, const FormatParams& p)
{
  bool negative = false;
  std::make_unsigned_t<T> number;
  if (number_orig < 0) {
    negative = true;
    number = -number_orig;
  } else {
    number = number_orig;
  }

  fixed_rstring<32> out;

  if (number == 0) {
    out.prepend('0');
  } else {
    while (number >= table_size) {
      out.prepend(table3[number % table_size]);
      if (p.comma) { out.prepend(','); }
      number /= table_size;
    }

    while (number > 0) {
      out.prepend('0' + (number % 10));
      number /= 10;
    }
  }

  if (negative) {
    out.prepend('-');
  } else if (p.sign) {
    out.prepend('+');
  }
  return out.to_string();
}

} // namespace

std::string to_string_t<int>::convert(int number, const FormatParams& p)
{
  return format_int<int>(number, p);
}

std::string to_string_t<unsigned>::convert(
  unsigned number, const FormatParams& p)
{
  return format_int<unsigned>(number, p);
}

std::string to_string_t<short>::convert(short number, const FormatParams& p)
{
  return format_int<short>(number, p);
}

std::string to_string_t<unsigned short>::convert(
  unsigned short number, const FormatParams& p)
{
  return format_int<unsigned short>(number, p);
}

std::string to_string_t<long>::convert(long number, const FormatParams& p)
{
  return format_int<long>(number, p);
}

std::string to_string_t<unsigned long>::convert(
  unsigned long number, const FormatParams& p)
{
  return format_int<unsigned long>(number, p);
}

std::string to_string_t<long long>::convert(
  long long number, const FormatParams& p)
{
  return format_int<long long>(number, p);
}

std::string to_string_t<unsigned long long>::convert(
  unsigned long long number, const FormatParams& p)
{
  return format_int<unsigned long long>(number, p);
}

} // namespace bee
