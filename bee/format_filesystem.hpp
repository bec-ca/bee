#pragma once

#include <filesystem>
#include <string>

#include "format.hpp"
#include "to_string_t.hpp"

namespace bee {

template <> struct to_string_t<std::filesystem::path> {
  static std::string convert(const std::filesystem::path& value)
  {
    return value;
  }
};

} // namespace bee
