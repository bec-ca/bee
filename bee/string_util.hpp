#pragma once

#include "error.hpp"

#include <string>
#include <vector>

namespace bee {

std::vector<std::string> split(const std::string& str, const std::string& sep);

std::vector<std::string> split_space(const std::string& str);

template <class T> std::string join(const T& container, const std::string& sep)
{
  std::string output;
  for (const auto& str : container) {
    if (!output.empty()) { output += sep; }
    output += str;
  }
  return output;
}

std::string right_pad_string(std::string str, size_t length);

std::string to_hex(const std::string& str);

OrError<std::string> of_hex(const std::string& str);

bool ends_with(std::string const& value, std::string const& ending);

template <class... Ts> void concat(std::string& output, Ts&&... parts)
{
  ([&]() { output += parts; }(), ...);
}

std::string remove_prefix(const std::string& str, const std::string& prefix);

} // namespace bee
