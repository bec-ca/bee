#pragma once

#include "to_string.hpp"

#include <filesystem>
#include <string>

namespace bee {

template <> struct to_string<std::filesystem::path> {
  static std::string convert(const std::filesystem::path& value)
  {
    return value;
  }
};

} // namespace bee
