#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

#include "bee/fixed_rstring.hpp"

namespace bee {

struct Hex {
  static char digit(int value);

  template <class T, size_t S>
    requires std::is_unsigned_v<T> && (S >= sizeof(T) * 2)
  static void to_hex_rstring(fixed_rstring<S>& out, T value)
  {
    do {
      out.prepend(Hex::digit(value % 16));
      value /= 16;
    } while (value > 0);
  }

  template <class T> static std::string to_hex_string(T value)
  {
    using U = std::make_unsigned_t<T>;
    fixed_rstring<sizeof(T) * 2> out;
    Hex::to_hex_rstring<U>(out, value);
    return out.to_string();
  }
};

} // namespace bee
