#include "hex_encoding.hpp"

#include <cstdint>

namespace bee {
namespace {

std::optional<uint8_t> of_hex_digit(char d)
{
  if (d >= '0' && d <= '9') return d - '0';
  if (d >= 'a' && d <= 'f') return d - 'a' + 10;
  if (d >= 'A' && d <= 'F') return d - 'A' + 10;
  return std::nullopt;
}

} // namespace

std::string HexEncoding::to_hex(const std::string_view& str)
{
  auto hex_digit = [](int value) {
    if (value < 10) return value + '0';
    return value - 10 + 'a';
  };
  std::string out;
  for (uint8_t c : str) {
    std::string k(2, ' ');
    k[0] = hex_digit(c / 0x10);
    k[1] = hex_digit(c % 0x10);
    out += k;
  }
  return out;
}

OrError<std::string> HexEncoding::of_hex(const std::string_view& str)
{
  if (str.size() % 2 != 0) {
    return Error("Hex string must have even number of characters");
  }

  std::string out;
  for (size_t i = 0; i < str.size(); i += 2) {
    auto d1 = of_hex_digit(str[i]);
    auto d2 = of_hex_digit(str[i + 1]);
    if (!d1.has_value() || !d2.has_value()) {
      return Error("Not a valid hex string");
    }
    uint8_t v = *d1 * 16 + *d2;
    out.push_back(v);
  }
  return out;
}

} // namespace bee
