#pragma once

#include <set>
#include <string>

#include "format.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::set<T>> {
  static std::string convert(const std::set<T>& values)
  {
    return convert_container(values);
  }
};

} // namespace bee
