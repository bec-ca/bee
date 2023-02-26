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

} // namespace bee
