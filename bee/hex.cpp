#include "hex.hpp"

#include <algorithm>

#include "fixed_rstring.hpp"

namespace bee {
namespace {

template <class T>
  requires std::is_unsigned_v<T>
std::string of_unsigned_padded(T value)
{
  fixed_rstring<32> out;
  Hex::to_hex_rstring<T>(out, value);
  while (out.size() < sizeof(T) * 2) { out.prepend('0'); }
  return out.to_string();
}

} // namespace

char Hex::digit(int value)
{
  if (value < 10) return value + '0';
  return value - 10 + 'a';
}

} // namespace bee
