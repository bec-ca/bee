#pragma once

#include <cstddef>

namespace bee {

template <class T, size_t S> constexpr size_t array_length(T (&)[S])
{
  return S;
}

} // namespace bee
