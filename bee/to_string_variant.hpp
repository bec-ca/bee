#pragma once

#include <string>
#include <variant>

#include "to_string_t.hpp"
#include "type_name.hpp"

namespace bee {

template <class... T> struct to_string_t<std::variant<T...>> {
  static std::string convert(const std::variant<T...>& value)
  {
    return std::visit(
      []<class V>(const V& v) {
        std::string out("[");
        out += TypeName<V>::name();
        out += " ";
        out += to_string(v);
        out += "]";
        return out;
      },
      value);
  }
};

} // namespace bee
