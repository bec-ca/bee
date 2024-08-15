#include "int_to_string.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <string>
#include <type_traits>

#include "fixed_rstring.hpp"
#include "format_params.hpp"
#include "hex.hpp"
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
  using U = std::make_unsigned_t<T>;
  U number;
  if (number_orig < 0) {
    negative = true;
    number = -number_orig;
  } else {
    number = number_orig;
  }

  fixed_rstring<32> out;

  if (p.hex) {
    Hex::to_hex_rstring<U>(out, number);
  } else if (number == 0) {
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

  if (p.left_pad_zeroes > 0) {
    while (out.ssize() < p.left_pad_zeroes) { out.prepend('0'); }
  }

  if (p.left_pad_spaces > 0) {
    while (out.ssize() < p.left_pad_spaces) { out.prepend(' '); }
  }

  if (negative) {
    out.prepend('-');
  } else if (p.sign) {
    out.prepend('+');
  }
  return out.to_string();
}

} // namespace

#define IMPLEMENT_CONVERTER(T)                                                 \
  std::string to_string_t<T>::convert(T number, const FormatParams& p)         \
  {                                                                            \
    return format_int<T>(number, p);                                           \
  }

IMPLEMENT_CONVERTER(signed char);
IMPLEMENT_CONVERTER(unsigned char);
IMPLEMENT_CONVERTER(short);
IMPLEMENT_CONVERTER(unsigned short);
IMPLEMENT_CONVERTER(int);
IMPLEMENT_CONVERTER(unsigned);
IMPLEMENT_CONVERTER(long);
IMPLEMENT_CONVERTER(unsigned long);
IMPLEMENT_CONVERTER(long long);
IMPLEMENT_CONVERTER(unsigned long long);

} // namespace bee
