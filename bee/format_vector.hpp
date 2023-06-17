#pragma once

#include <string>
#include <vector>

#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::vector<T>> {
  static std::string convert(const std::vector<T>& values)
  {
    return convert_container(values);
  }
};

}; // namespace bee
