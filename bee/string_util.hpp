#pragma once

#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "to_string.hpp"

namespace bee {

std::vector<std::string> split(
  const std::string_view& str, const std::string_view& sep);

// Split string by spaces characters. Consecutive spaces are treated as a
// single space, leading and trailing spaces are ignored. Space characters
// include regular space, tab, new line, etc.
std::vector<std::string> split_space(
  const std::string_view& str,
  const int max_parts = std::numeric_limits<int>::max());

std::vector<std::string> split_lines(const std::string_view& str);

template <class T> std::string join(const T& container)
{
  std::string output;
  for (const auto& v : container) { output += to_string(v); }
  return output;
}

template <class T>
std::string join(const T& container, const std::string_view& sep)
{
  std::string output;
  bool is_first = true;
  for (const auto& v : container) {
    if (!is_first) { output += sep; }
    is_first = false;
    output += to_string(v);
  }
  return output;
}

template <class T>
std::string join_lines(const T& container, const std::string_view& sep)
{
  std::string output;
  for (const auto& v : container) {
    output += v;
    output += sep;
  }
  return output;
}

std::string right_pad_string(std::string str, size_t length);

template <class... Ts> void concat(std::string& output, Ts&&... parts)
{
  ([&]() { output += parts; }(), ...);
}

std::optional<std::string> remove_prefix(
  const std::string_view& str, const std::string_view& prefix);

std::optional<std::string> remove_suffix(
  const std::string_view& str, const std::string_view& prefix);

std::string find_and_replace(
  const std::string_view& str,
  const std::string_view& needle,
  const std::string_view& rep);

// [trim] removes leading and trailing spaces
std::string trim_spaces(const std::string_view& str);

bool contains_string(
  const std::string_view& str, const std::string_view& needle);

std::string to_lower(const std::string_view& str);
std::string to_upper(const std::string_view& str);

} // namespace bee
