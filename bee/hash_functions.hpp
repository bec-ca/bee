#pragma once

#include <cstdint>
#include <string>

namespace bee {

struct HashFunctions {
  static uint32_t simple32(uint32_t x);
  static uint64_t simple64(uint64_t x);

  static uint64_t compose64(uint64_t h1, uint64_t h2);

  static uint64_t simple_string_hash(const std::string& str);
};

} // namespace bee
