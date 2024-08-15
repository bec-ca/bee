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

namespace to_string_t_details {

template <class T>
concept has_to_string = requires(const T& t) {
  { t.to_string() } -> std::convertible_to<std::string>;
};

template <class T>
concept has_to_string_with_params =
  requires(const T& t, const FormatParams& p) {
    { t.to_string(p) } -> std::convertible_to<std::string>;
  };

} // namespace to_string_t_details

template <to_string_t_details::has_to_string T>
  requires(!to_string_t_details::has_to_string_with_params<T>)
struct to_string_t<T> {
  static std::string convert(const T& value) { return value.to_string(); }
  static std::string convert(T&& value) { return std::move(value).to_string(); }
};

template <to_string_t_details::has_to_string_with_params T>
struct to_string_t<T> {
  static std::string convert(const T& value, const FormatParams& params = {})
  {
    return value.to_string(params);
  }

  static std::string convert(T&& value, const FormatParams& params = {})
  {
    return std::move(value).to_string(params);
  }
};

} // namespace bee
