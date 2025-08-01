#pragma once

#include <optional>
#include <string>

#include "format_params.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::optional<T>> {
  static std::string convert(
    const std::optional<T>& value, const FormatParams& p)
  {
    if (value.has_value()) {
      return to_string(*value, p);
    } else {
      return "<nullopt>";
    }
  }
};

} // namespace bee
