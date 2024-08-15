#pragma once

#include <optional>
#include <string>

#include "to_string.hpp"
#include "to_string_t.hpp"

#include "bee/format_params.hpp"

namespace bee {

template <class T> struct to_string_t<std::optional<T>> {
  static std::string convert(
    const std::optional<T>& value, const FormatParams& p)
  {
    if (value.has_value()) {
      return bee::to_string(*value, p);
    } else {
      return "<nullopt>";
    }
  }
};

} // namespace bee
