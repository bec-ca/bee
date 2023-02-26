#pragma once

#include <compare>
#include <string>

namespace bee {

template <class T> struct StringMixin {
 public:
  static T of_string(const std::string& str) { return T(str); }
  static T of_string(std::string&& str) { return T(std::move(str)); }

  const std::string& to_string() const { return _str; }

  std::strong_ordering operator<=>(const StringMixin& other) const = default;

  explicit StringMixin(const std::string& str) : _str(str) {}
  explicit StringMixin(std::string&& str) : _str(std::move(str)) {}

 private:
  std::string _str;
};

}; // namespace bee
