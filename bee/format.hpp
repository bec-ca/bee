#pragma once

#include <cassert>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

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

void raise_exn(const char* file, int line, const char* format, const char* msg);

std::optional<FormatParams> get_next_format(
  const char* file,
  int line,
  std::string& output,
  const char* format,
  int& idx);

template <class... Ts>
void format_rec2(
  const char* file,
  int line,
  const char* format,
  int idx,
  std::string& output,
  Ts&&... args);

template <class T, class... Ts>
void write_one_rec(
  const char* file,
  int line,
  const char* format,
  int idx,
  const FormatParams& params,
  std::string& output,
  T&& v1,
  Ts&&... args)
{
  output += to_string(v1, params);
  format_rec2(file, line, format, idx, output, std::forward<Ts>(args)...);
}

void write_one_rec(
  const char* file,
  int line,
  const char*,
  int,
  const FormatParams&,
  std::string&);

template <class... Ts>
void format_rec2(
  const char* file,
  int line,
  const char* format,
  int idx,
  std::string& output,
  Ts&&... args)
{
  if (auto params = get_next_format(file, line, output, format, idx)) {
    write_one_rec(
      file, line, format, idx, *params, output, std::forward<Ts>(args)...);
  } else if constexpr (sizeof...(Ts) > 0) {
    raise_exn(file, line, format, "Extra arguments in format");
  }
}

template <class T> std::string format(const char*, int, T&& v)
{
  return to_string(v);
}

template <class... Ts>
std::string format(const char* file, int line, const char* f, Ts&&... args)
{
  std::string output;
  format_rec2(file, line, f, 0, output, std::forward<Ts>(args)...);
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// stdout
//

void print_str(std::string str);

inline void print_line(const std::string& str) { print_str(str); }
inline void print_line(std::string&& str) { print_str(std::move(str)); }

template <class T> void print_line(const char* file, int line, T&& v)
{
  print_str(bee::format_details::format(file, line, std::forward<T>(v)));
}

template <class... Ts>
void print_line(const char* file, int line, const char* f, Ts&&... args)
{
  print_str(
    bee::format_details::format(file, line, f, std::forward<Ts>(args)...));
}

////////////////////////////////////////////////////////////////////////////////
// stderr
//

void print_err_str(std::string str);

inline void print_err_line(const char*, int, const std::string& str)
{
  print_err_str(str);
}

inline void print_err_line(const char*, int, std::string&& str)
{
  print_err_str(std::move(str));
}

template <class T> void print_err_line(const char* file, int line, T&& v)
{
  print_err_str(bee::format_details::format(file, line, std::forward<T>(v)));
}

template <class... Ts>
void print_err_line(const char* file, int line, const char* f, Ts... args)
{
  print_err_str(bee::format_details::format(file, line, f, args...));
}

} // namespace format_details

////////////////////////////////////////////////////////////////////////////////
// macros
//

#define F(...) ::bee::format_details::format(__FILE__, __LINE__, __VA_ARGS__)

#define P(...)                                                                 \
  ::bee::format_details::print_line(__FILE__, __LINE__, __VA_ARGS__)

#define PE(...)                                                                \
  ::bee::format_details::print_err_line(__FILE__, __LINE__, __VA_ARGS__)

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
