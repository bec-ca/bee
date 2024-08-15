#pragma once

#include <concepts>

namespace bee {

template <class T>
concept always_false = false;

template <class T, typename... Us>
concept is_one_of = (std::same_as<T, Us> || ...);

} // namespace bee
