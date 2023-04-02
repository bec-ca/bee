#pragma once

#include "to_string.hpp"

#include <array>
#include <string>

namespace bee {

template <class T, size_t S> struct to_string<std::array<T, S>> {
  static std::string convert(const std::array<T, S>& values)
  {
    return convert_container(values);
  }
};

} // namespace bee
