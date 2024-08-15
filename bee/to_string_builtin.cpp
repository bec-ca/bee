#include "to_string_builtin.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <type_traits>

#include "hex.hpp"
#include "to_string_t.hpp"

namespace bee {

// bool

const char* to_string_t<bool>::convert(bool value)
{
  return value ? "true" : "false";
}

// char

std::string to_string_t<char>::convert(char value)
{
  return std::string(1, value);
}

// std::string

std::string to_string_t<std::string>::convert(
  const std::string& value, const FormatParams& p)
{
  if (p.left_pad_spaces > 0 && std::ssize(value) < p.left_pad_spaces) {
    std::string copy = value;
    copy.resize(p.left_pad_spaces, ' ');
    return copy;
  } else {
    return value;
  }
}

std::string to_string_t<std::string>::convert(
  std::string&& value, const FormatParams& p)
{
  if (p.left_pad_spaces > 0 && std::ssize(value) < p.left_pad_spaces) {
    return convert(value, p);
  } else {
    return std::move(value);
  }
}

// void*

std::string to_string_t<void*>::convert(const void* value)
{
  return to_string_t<const void*>::convert(value);
}

// const void*

std::string to_string_t<const void*>::convert(const void* value)
{
  return bee::Hex::to_hex_string(reinterpret_cast<uintptr_t>(value));
  std::ostringstream ss;
  ss << std::hex << value;
  return ss.str();
}

// string_view

std::string to_string_t<std::string_view>::convert(
  const std::string_view& value)
{
  return std::string(value);
}

} // namespace bee
