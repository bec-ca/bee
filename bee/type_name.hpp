#pragma once

#include <concepts>
#include <string>
#include <typeinfo>

namespace bee {

template <class F>
concept has_type_name = requires(F f) {
  { F::type_name() } -> std::convertible_to<const char*>;
};

template <class T> struct TypeName;

template <has_type_name T> struct TypeName<T> {
 public:
  static const char* name() { return T::type_name(); }
};

template <class T> struct TypeName {
 public:
  static const char* name() { return typeid(T).name(); }
};

template <> struct TypeName<std::string> {
 public:
  static const char* name() { return "std::string"; }
};

template <> struct TypeName<int> {
 public:
  static const char* name() { return "int"; }
};

} // namespace bee
