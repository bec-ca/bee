#include "hash_functions.hpp"

#include <cstdint>

namespace bee {

uint64_t HashFunctions::simple_string_hash(const std::string& str)
{
  uint64_t output = 0;
  uint64_t acc = 0;
  int counter = 0;

  auto flush = [&]() {
    counter = 0;
    output = simple64(output ^ (simple64(acc)));
    acc = 0;
  };

  for (char c : str) {
    acc = (acc << 8) | (static_cast<uint8_t>(c));
    counter++;
    if (counter == 8) { flush(); }
  }
  if (counter > 0) { flush(); }
  return output;
}

// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
uint32_t HashFunctions::simple32(uint32_t x)
{
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

uint64_t HashFunctions::simple64(uint64_t x)
{
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ll;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebll;
  x = x ^ (x >> 31);
  return x;
}

uint64_t HashFunctions::compose64(uint64_t h1, uint64_t h2)
{
  return simple64(simple64(h1) ^ simple64(h2));
}

} // namespace bee
