#pragma once

#include "or_error.hpp"

namespace bee {

template <class T> struct IntOfString;

#define DECLARE_PARSER(T)                                                      \
  template <> struct IntOfString<T> {                                          \
    static OrError<T> parse(const std::string_view& t);                        \
  };

DECLARE_PARSER(int);
DECLARE_PARSER(unsigned);

DECLARE_PARSER(unsigned long);
DECLARE_PARSER(long);

DECLARE_PARSER(unsigned long long);
DECLARE_PARSER(long long);

#undef DECLARE_PARSER

template <class T>
concept HasIntOfString =
  requires(const std::string_view& t) { IntOfString<T>::parse(t); };

} // namespace bee
