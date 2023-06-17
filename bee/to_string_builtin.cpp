#include "to_string_builtin.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <type_traits>

#include "to_string_t.hpp"

namespace bee {

std::string to_string_t<char>::convert(char value)
{
  return std::string(1, value);
}

std::string to_string_t<signed char>::convert(signed char value)
{
  return std::to_string(int(value));
}

std::string to_string_t<unsigned char>::convert(unsigned char value)
{
  return std::to_string(int(value));
}

std::string to_string_t<std::string>::convert(const std::string& value)
{
  return value;
}

std::string to_string_t<const char*>::convert(const char* value)
{
  return value;
}

std::string to_string_t<bool>::convert(bool value)
{
  return value ? "true" : "false";
}

std::string to_string_t<char*>::convert(const char* value) { return value; }

std::string to_string_t<void*>::convert(const void* value)
{
  return to_string_t<const void*>::convert(value);
}

std::string to_string_t<const void*>::convert(const void* value)
{
  std::ostringstream ss;
  ss << std::hex << value;
  return ss.str();
}

} // namespace bee
