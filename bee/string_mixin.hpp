#pragma once

#include <compare>
#include <string>

namespace bee {

template <class T> struct StringMixin {
 public:
  template <class U>
    requires std::constructible_from<std::string, U>
  static T of_string(U&& str)
  {
    return static_cast<T>(std::forward<U>(str));
  }

  const std::string& to_string() const { return _str; }

  std::strong_ordering operator<=>(const StringMixin& other) const = default;

  template <class U>
    requires std::constructible_from<std::string, U>
  explicit StringMixin(U&& str) : _str(std::forward<U>(str))
  {}

 private:
  std::string _str;
};

}; // namespace bee
