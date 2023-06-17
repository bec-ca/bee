#pragma once

#include <string>

#include "concepts.hpp"
#include "to_string_builtin.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T>
concept convertible_to_string = requires(const T& t) {
                                  {
                                    to_string_t<std::decay_t<T>>::convert(t)
                                    } -> std::convertible_to<std::string>;
                                };

template <class T>
concept convertible_to_string_with_params =
  requires(const T& t, const FormatParams& p) {
    {
      to_string_t<std::decay_t<T>>::convert(t, p)
      } -> std::convertible_to<std::string>;
  };

template <class T>
auto to_string(const T& v, const FormatParams& p = FormatParams())
{
  if constexpr (convertible_to_string_with_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(v, p);
  } else if constexpr (convertible_to_string<T>) {
    return to_string_t<std::decay_t<T>>::convert(v);
  } else {
    static_assert(always_false_v<T>, "No string conversion found for type");
  }
}

} // namespace bee
