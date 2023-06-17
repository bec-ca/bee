#pragma once

#include <string>

#include "float_to_string.hpp"
#include "int_to_string.hpp"
#include "to_string_t.hpp"

namespace bee {

template <> struct to_string_t<char> {
  static std::string convert(char value);
};

template <> struct to_string_t<signed char> {
  static std::string convert(signed char value);
};

template <> struct to_string_t<unsigned char> {
  static std::string convert(unsigned char value);
};

template <> struct to_string_t<std::string> {
  static std::string convert(const std::string& value);
};

template <> struct to_string_t<const char*> {
  static std::string convert(const char* value);
};

template <> struct to_string_t<char*> {
  static std::string convert(const char* value);
};

template <> struct to_string_t<bool> {
  static std::string convert(bool value);
};

template <> struct to_string_t<void*> {
  static std::string convert(const void* value);
};

template <> struct to_string_t<const void*> {
  static std::string convert(const void* value);
};

} // namespace bee
