#pragma once

#include "to_string.hpp"
#include "type_name.hpp"

#include <string>
#include <variant>

namespace bee {

template <class... T> struct to_string<std::variant<T...>> {
  static std::string convert(const std::variant<T...>& value)
  {
    return std::visit(
      []<class V>(const V& v) {
        return format("[$ $]", TypeName<V>::name(), v);
      },
      value);
  }
};

} // namespace bee
