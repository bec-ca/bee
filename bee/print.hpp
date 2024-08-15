#pragma once

#include <string>

#include "format.hpp"
#include "location.hpp"
#include "log_output.hpp"
#include "to_string.hpp"

namespace bee {

namespace details {

void print_file_str(LogOutput out, const std::string& str);

inline void print_file_line(
  LogOutput out, const Location&, const std::string& str)
{
  print_file_str(out, str);
}

template <stringable T>
void print_file_line(LogOutput out, const Location& loc, T&& v)
{
  print_file_str(out, bee::format_details::format(loc, std::forward<T>(v)));
}

template <stringable... Ts>
void print_file_line(
  LogOutput out, const Location& loc, const char* fmt, Ts&&... args)
{
  print_file_str(
    out, bee::format_details::format(loc, fmt, std::forward<Ts>(args)...));
}

} // namespace details

////////////////////////////////////////////////////////////////////////////////
// macros
//

#define PF(output, args...) ::bee::details::print_file_line(output, HERE, args)

#define P(args...) PF(::bee::LogOutput::StdOut, args)

#define PE(args...) PF(::bee::LogOutput::StdErr, args)

#define log_and_return_void(var, or_error, msg...)                             \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var).error().add_tag_with_location(HERE, bee::maybe_format(msg));  \
    PE((__var##var).error().full_msg());                                       \
    return;                                                                    \
  }                                                                            \
  auto& var = (__var##var).value()

#define log_error(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(HERE, bee::maybe_format(msg));       \
      PE((__var).error().full_msg());                                          \
    }                                                                          \
  } while (false)

void flush_stdout();
void flush_stderr();

} // namespace bee
