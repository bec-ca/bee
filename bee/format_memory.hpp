#pragma once

#include <memory>
#include <string>

#include "format.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::shared_ptr<T>> {
  static std::string convert(const std::shared_ptr<T>& value)
  {
    if (value) {
      return F(*value);
    } else {
      return "nullptr";
    }
  }
};

template <class T> struct to_string_t<std::unique_ptr<T>> {
  static std::string convert(const std::unique_ptr<T>& value)
  {
    if (value) {
      return F(*value);
    } else {
      return "nullptr";
    }
  }
};

} // namespace bee
