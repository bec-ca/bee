#pragma once

#include <concepts>
#include <typeinfo>

namespace bee {

template <class F>
concept has_type_name = requires(F f) {
  {
    F::type_name()
  } -> std::convertible_to<const char*>;
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

} // namespace bee
