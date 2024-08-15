#pragma once

#include "or_error.hpp"

namespace bee {

template <class T> struct FloatOfString;

#define DECLARE_PARSER(T)                                                      \
  template <> struct FloatOfString<T> {                                        \
    static OrError<T> parse(const std::string_view& t);                        \
  };

DECLARE_PARSER(float);
DECLARE_PARSER(double);

#undef DECLARE_PARSER

template <class T>
concept HasFloatOfString =
  requires(const std::string_view& t) { FloatOfString<T>::parse(t); };

} // namespace bee
