#pragma once

#include <concepts>
#include <string>

#include "concepts.hpp"
#include "format_params.hpp"

namespace bee {

template <class T> struct to_string_t;

template <class T> struct to_string_t {
  static_assert(always_false<T>, "No string conversion found for type");
};

namespace details {

template <class T>
concept has_to_string = requires(const T& t) {
  { t.to_string() } -> std::convertible_to<std::string>;
};

template <class T>
concept has_to_string_with_params =
  requires(const T& t, const FormatParams& p) {
    { t.to_string(p) } -> std::convertible_to<std::string>;
  };

} // namespace details

template <details::has_to_string T>
  requires(!details::has_to_string_with_params<T>)
struct to_string_t<T> {
  template <class U> static decltype(auto) convert(U&& value)
  {
    return std::forward<U>(value).to_string();
  }
};

template <details::has_to_string_with_params T> struct to_string_t<T> {
  template <class U>
  static decltype(auto) convert(U&& value, const FormatParams& params = {})
  {
    return std::forward<U>(value).to_string(params);
  }
};

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

template <stringable T> decltype(auto) to_string(T&& v, const FormatParams& p)
{
  if constexpr (convertible_to_string_with_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(std::forward<T>(v), p);
  } else if constexpr (convertible_to_string_without_params<T>) {
    return to_string_t<std::decay_t<T>>::convert(std::forward<T>(v));
  } else {
    static_assert(always_false<T>, "No string conversion found for type");
  }
}

template <stringable T> decltype(auto) to_string(T&& v)
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

template <class T> std::string format_container(const T& container)
{
  std::string output;
  for (const auto& el : container) {
    if (!output.empty()) { output += " "; }
    output += to_string(el);
  }
  return output;
}

} // namespace bee
