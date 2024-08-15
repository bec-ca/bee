#pragma once

#include <string>

#include "float_to_string.hpp"
#include "int_to_string.hpp"
#include "to_string_t.hpp"

#include "bee/format_params.hpp"

namespace bee {

template <> struct to_string_t<bool> {
  static const char* convert(bool value);
};

template <> struct to_string_t<char> {
  static std::string convert(char value);
};

template <> struct to_string_t<std::string> {
  static std::string convert(const std::string& value, const FormatParams&);
  static std::string convert(std::string&& value, const FormatParams&);

  template <class T> static auto&& convert(T&& value)
  {
    return std::forward<T>(value);
  }
};

template <> struct to_string_t<char*> {
  static char* convert(char* value) { return value; }
};

template <> struct to_string_t<const char*> {
  static const char* convert(const char* value) { return value; }
};

template <> struct to_string_t<void*> {
  static std::string convert(const void* value);
};

template <> struct to_string_t<const void*> {
  static std::string convert(const void* value);
};

template <> struct to_string_t<std::string_view> {
  static std::string convert(const std::string_view& value);
};

} // namespace bee
