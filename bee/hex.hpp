#pragma once

#include <string>

namespace bee {

struct Hex {
  static char digit(int value)
  {
    if (value < 10) return value + '0';
    return value - 10 + 'a';
  }

  static std::string of_uint64(uint64_t value)
  {
    std::string output;
    for (int i = 0; i < 16; i++) {
      output += digit(value % 16);
      value /= 16;
    }
    return output;
  }
};

} // namespace bee
