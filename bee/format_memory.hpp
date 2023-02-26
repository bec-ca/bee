#pragma once

#include "to_string.hpp"

#include <memory>
#include <string>

namespace bee {

template <class T> struct to_string<std::shared_ptr<T>> {
  static std::string convert(const std::shared_ptr<T>& value)
  {
    if (value) {
      return format(*value);
    } else {
      return "nullptr";
    }
  }
};

template <class T> struct to_string<std::unique_ptr<T>> {
  static std::string convert(const std::unique_ptr<T>& value)
  {
    if (value) {
      return format(*value);
    } else {
      return "nullptr";
    }
  }
};

} // namespace bee
