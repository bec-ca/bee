#pragma once

#include "bee/float_of_string.hpp"
#include "bee/int_of_string.hpp"

namespace bee {

template <HasIntOfString T> OrError<T> parse_string(const std::string_view& str)
{
  return IntOfString<T>::parse(str);
}

template <HasFloatOfString T>
OrError<T> parse_string(const std::string_view& str)
{
  return FloatOfString<T>::parse(str);
}

} // namespace bee
