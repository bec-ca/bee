#pragma once

#include <cassert>
#include <string>
#include <type_traits>

#include "to_string.hpp"

namespace bee {

template <> struct to_string<int> {
  static std::string convert(int value) { return std::to_string(value); }
};

template <> struct to_string<unsigned int> {
  static std::string convert(unsigned int value)
  {
    return std::to_string(value);
  }
};

template <> struct to_string<short unsigned int> {
  static std::string convert(short unsigned int value)
  {
    return std::to_string(value);
  }
};

template <> struct to_string<long> {
  static std::string convert(long value) { return std::to_string(value); }
};

template <> struct to_string<long long> {
  static std::string convert(long long value) { return std::to_string(value); }
};

template <> struct to_string<unsigned long> {
  static std::string convert(unsigned long value)
  {
    return std::to_string(value);
  }
};

template <> struct to_string<unsigned long long> {
  static std::string convert(unsigned long long value)
  {
    return std::to_string(value);
  }
};

template <> struct to_string<char> {
  static std::string convert(char value) { return std::string(1, value); }
};

template <> struct to_string<signed char> {
  static std::string convert(signed char value)
  {
    return std::to_string(int(value));
  }
};

template <> struct to_string<std::string> {
  static std::string convert(const std::string& value) { return value; }
};

template <> struct to_string<const char*> {
  static std::string convert(const char* value) { return value; }
};

template <> struct to_string<char*> {
  static std::string convert(const char* value) { return value; }
};

template <> struct to_string<bool> {
  static std::string convert(bool value) { return value ? "true" : "false"; }
};

template <> struct to_string<float> {
  static std::string convert(float value) { return std::to_string(value); }
};

template <> struct to_string<double> {
  static std::string convert(double value) { return std::to_string(value); }
};

template <> struct to_string<void*> {
  static std::string convert(const void* value);
};

template <> struct to_string<const void*> {
  static std::string convert(const void* value);
};

template <class T> struct to_string {
  static std::string convert(const T& value) { return value.to_string(); }
};

template <class T> std::string convert_container(const T& container)
{
  std::string output;
  for (const auto& el : container) {
    if (!output.empty()) { output += " "; }
    output += to_string<typename T::value_type>::convert(el);
  }
  return output;
}

template <class T> std::string convert_one(const T& value)
{
  return to_string<std::decay_t<T>>::convert(value);
}

template <class T, class... Ts>
void format_rec(const char* format, std::string& output, T&& v1, Ts&&... args)
{
  while (*format) {
    if (*format == '$') {
      format++;
      if (*format == '$') {
        output += '$';
        format++;
      } else {
        output += to_string<std::decay_t<T>>::convert(v1);
        if constexpr ((sizeof...(Ts)) > 0) {
          format_rec(format, output, std::forward<Ts>(args)...);
        } else {
          output += format;
        }
        return;
      }
    } else {
      output += *format;
      format++;
    }
  }
  if constexpr ((sizeof...(Ts)) > 0) {
    assert(false && "extra unused format arguments");
  }
}

template <class T> std::string format(const T& v)
{
  return to_string<std::decay_t<T>>::convert(v);
}

template <class... Ts> std::string format(const char* f, Ts&&... args)
{
  if constexpr ((sizeof...(Ts)) == 0) {
    return f;
  } else {
    std::string output;
    format_rec(f, output, std::forward<Ts>(args)...);
    return output;
  }
}

////////////////////////////////////////////////////////////////////////////////
// stdout
//

void print_str(std::string str);

inline void print_line(const std::string& str) { print_str(str); }
inline void print_line(std::string&& str) { print_str(std::move(str)); }

template <class T> void print_line(T&& v)
{
  print_str(format(std::forward<T>(v)));
}

template <class... Ts> void print_line(const char* f, Ts&&... args)
{
  print_str(format(f, std::forward<Ts>(args)...));
}

////////////////////////////////////////////////////////////////////////////////
// stderr
//

void print_err_str(std::string str);

inline void print_err_line(const std::string& str) { print_err_str(str); }
inline void print_err_line(std::string&& str) { print_err_str(std::move(str)); }

template <class T> void print_err_line(T&& v)
{
  print_err_str(format(std::forward<T>(v)));
}

template <class... Ts> void print_err_line(const char* f, Ts... args)
{
  print_err_str(format(f, args...));
}

template <class T, class F> struct to_string<std::pair<T, F>> {
  static std::string convert(const std::pair<T, F>& p)
  {
    return format("($, $)", p.first, p.second);
  }
};

} // namespace bee
