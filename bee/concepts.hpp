#pragma once

#include <concepts>

#include "unit.hpp"

namespace bee {

template <class> constexpr bool always_false_v = false;

////////////////////////////////////////////////////////////////////////////////
// is_one_of
//

template <typename... Us> struct is_one_of;

template <typename T, typename U, typename... Us>
struct is_one_of<T, U, Us...> {
 private:
  constexpr static bool _value()
  {
    if constexpr (std::is_same_v<T, U>)
      return true;
    else if constexpr ((sizeof...(Us)) > 0)
      return is_one_of<T, Us...>::value;
    else
      return false;
  }

 public:
  constexpr static bool value = _value();
};

template <typename... Us> constexpr bool is_one_of_v = is_one_of<Us...>::value;

} // namespace bee
