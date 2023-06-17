#pragma once

#include <array>
#include <string>

#include "to_string_t.hpp"

namespace bee {

template <class T, size_t S> struct to_string_t<std::array<T, S>> {
  static std::string convert(const std::array<T, S>& values)
  {
    return convert_container(values);
  }
};

} // namespace bee
