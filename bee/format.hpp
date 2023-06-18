#pragma once

#include <cassert>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "location.hpp"
#include "to_string.hpp"

namespace bee {

template <class T> std::string convert_container(const T& container)
{
  std::string output;
  for (const auto& el : container) {
    if (!output.empty()) { output += " "; }
    output += to_string(el);
  }
  return output;
}

namespace format_details {

void raise_exn(const Location& loc, const char* fmt, const char* msg);

std::optional<FormatParams> get_next_format(
  const Location& loc, std::string& output, const char* fmt, int& idx);

template <class... Ts>
void format_rec2(
  const Location& loc,
  const char* fmt,
  int idx,
  std::string& output,
  Ts&&... args);

template <class T, class... Ts>
void write_one_rec(
  const Location& loc,
  const char* fmt,
  int idx,
  const FormatParams& params,
  std::string& output,
  T&& v1,
  Ts&&... args)
{
  output += to_string(v1, params);
  format_rec2(loc, fmt, idx, output, std::forward<Ts>(args)...);
}

void write_one_rec(
  const Location& loc,
  const char* fmt,
  int idx,
  const FormatParams&,
  std::string&);

template <class... Ts>
void format_rec2(
  const Location& loc,
  const char* fmt,
  int idx,
  std::string& output,
  Ts&&... args)
{
  if (auto params = get_next_format(loc, output, fmt, idx)) {
    write_one_rec(loc, fmt, idx, *params, output, std::forward<Ts>(args)...);
  } else if constexpr (sizeof...(Ts) > 0) {
    raise_exn(loc, fmt, "Extra arguments in format");
  }
}

template <class T> std::string format(const Location&, T&& v)
{
  return to_string(v);
}

template <class... Ts>
std::string format(const Location& loc, const char* fmt, Ts&&... args)
{
  std::string output;
  format_rec2(loc, fmt, 0, output, std::forward<Ts>(args)...);
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// stdout
//

void print_str(std::string str);

inline void print_line(const std::string& str) { print_str(str); }
inline void print_line(std::string&& str) { print_str(std::move(str)); }

template <class T> void print_line(const Location& loc, T&& v)
{
  print_str(bee::format_details::format(loc, std::forward<T>(v)));
}

template <class... Ts>
void print_line(const Location& loc, const char* fmt, Ts&&... args)
{
  print_str(bee::format_details::format(loc, fmt, std::forward<Ts>(args)...));
}

////////////////////////////////////////////////////////////////////////////////
// stderr
//

void print_err_str(std::string str);

inline void print_err_line(const Location&, const std::string& str)
{
  print_err_str(str);
}

inline void print_err_line(const Location&, std::string&& str)
{
  print_err_str(std::move(str));
}

template <class T> void print_err_line(const Location& loc, T&& v)
{
  print_err_str(bee::format_details::format(loc, std::forward<T>(v)));
}

template <class... Ts>
void print_err_line(const Location& loc, const char* fmt, Ts... args)
{
  print_err_str(bee::format_details::format(loc, fmt, args...));
}

} // namespace format_details

////////////////////////////////////////////////////////////////////////////////
// macros
//

#define F(...) ::bee::format_details::format(HERE, __VA_ARGS__)

#define P(...) ::bee::format_details::print_line(HERE, __VA_ARGS__)

#define PE(...) ::bee::format_details::print_err_line(HERE, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// pair
//

template <class T, class F> struct to_string_t<std::pair<T, F>> {
  static std::string convert(const std::pair<T, F>& p)
  {
    return F("[$ $]", p.first, p.second);
  }
};

} // namespace bee
