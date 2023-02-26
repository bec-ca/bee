#pragma once

#include <cstdint>
#include <string>

namespace bee {

struct HashFunctions {
  // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
  static inline uint32_t simple32(uint32_t x)
  {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
  }

  static inline uint64_t simple64(uint64_t x)
  {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
  }

  static uint64_t simple_string_hash(const std::string& str);
};

} // namespace bee
