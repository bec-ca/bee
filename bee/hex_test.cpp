#include "hex.hpp"

#include "bee/testing.hpp"

namespace bee {
namespace {

TEST(digit)
{
  for (int i = 0; i < 16; i++) { P("$ -> $", i, Hex::digit(i)); }
}

TEST(uint8)
{
  auto values = {0, 10, 16, 128, 255};
  for (auto v : values) { P("$ -> $", v, Hex::to_hex_string(v)); }
}

TEST(uint64)
{
  auto values = {
    0ull, 1000000ull, 0xffffffull, 0xffffffffull, 0xffffffffffffffffull};
  for (auto v : values) { P("$ -> $", v, Hex::to_hex_string(v)); }
}

TEST(of_string)
{
  PRINT_EXPR(Hex::to_hex_string(uint8_t(10)));
  PRINT_EXPR(Hex::to_hex_string(uint16_t(10)));
  PRINT_EXPR(Hex::to_hex_string(uint32_t(10)));
  PRINT_EXPR(Hex::to_hex_string(uint64_t(10)));
}

} // namespace
} // namespace bee
