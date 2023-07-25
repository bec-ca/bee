#pragma once

#include <map>
#include <string>

#include "format.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T, class F> struct to_string_t<std::map<T, F>> {
  static std::string convert(const std::map<T, F>& values)
  {
    return convert_container(values);
  }
};

template <class T, class F> struct to_string_t<std::multimap<T, F>> {
  static std::string convert(const std::multimap<T, F>& values)
  {
    return convert_container(values);
  }
};

} // namespace bee
