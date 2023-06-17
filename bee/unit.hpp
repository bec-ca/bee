#pragma once

#include <string>

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// Unit
//

struct Unit {
  std::string to_string() const { return {}; }
};

constexpr Unit unit;

////////////////////////////////////////////////////////////////////////////////
// unit_if_void
//

template <class T> struct unit_if_void;

template <> struct unit_if_void<void> {
  using type = Unit;
};

template <class T> struct unit_if_void {
  using type = T;
};

template <class T> using unit_if_void_t = typename unit_if_void<T>::type;

} // namespace bee
