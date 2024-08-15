#pragma once

#include <string>

#include "concepts.hpp"
#include "to_string_builtin.hpp" // IWYU pragma: export
#include "to_string_t.hpp"

namespace bee {

template <class T>
concept convertible_to_string_without_params = requires(const T& t) {
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
concept stringable = convertible_to_string_with_params<T> ||
                     convertible_to_string_without_params<T>;

template <stringable T> auto to_string(T&& v, const FormatParams& p)
{
  if constexpr (convertible_to_string_with_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(std::forward<T>(v), p);
  } else if constexpr (convertible_to_string_without_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(std::forward<T>(v));
  } else {
    static_assert(always_false<T>, "No string conversion found for type");
  }
}

template <stringable T> auto to_string(T&& v)
{
  if constexpr (convertible_to_string_without_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(std::forward<T>(v));
  } else if constexpr (convertible_to_string_with_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(
      std::forward<T>(v), FormatParams());
  } else {
    static_assert(always_false<T>, "No string conversion found for type");
  }
}

} // namespace bee
