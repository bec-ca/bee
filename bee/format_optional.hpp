#pragma once

#include <optional>
#include <string>

#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::optional<T>> {
  static std::string convert(const std::optional<T>& value)
  {
    if (value.has_value()) {
      return F("($)", value.value());
    } else {
      return "()";
    }
  }
};

} // namespace bee
