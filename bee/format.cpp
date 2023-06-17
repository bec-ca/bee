#include "format.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

using std::cerr;
using std::cout;
using std::flush;
using std::string;

namespace bee {
namespace format_details {

void print_str(string str)
{
  str += '\n';
  cout << str << flush;
}

void print_err_str(string str)
{
  str += '\n';
  cerr << str << flush;
}

void raise_exn(const char* file, int line, const char* format, const char* msg)
{
  throw std::runtime_error(
    std::string(file) + ":" + std::to_string(line) + ": " + std::string(msg) +
    ": '" + format + "'");
}

void write_one_rec(
  const char* file,
  int line,
  const char* format,
  int,
  const FormatParams&,
  std::string&)
{
  raise_exn(file, line, format, "Extra formats in format string");
}

std::optional<FormatParams> get_next_format(
  const char* file, int line, std::string& output, const char* format, int& idx)
{
  while (format[idx]) {
    FormatParams params;
    if ((format[idx] == '$' && format[idx + 1] != '$')) {
      idx++;
      return FormatParams();
    } else if (format[idx] == '$' && format[idx + 1] == '$') {
      output += '$';
      idx += 2;
    } else if (format[idx] == '{' && format[idx + 1] == '{') {
      output += '{';
      idx += 2;
    } else if (format[idx] == '{') {
      idx++;
      while (format[idx] && format[idx] != '}') {
        switch (format[idx]) {
        case ',': {
          params.comma = true;
          idx++;
          break;
        }
        case '.': {
          idx++;
          int places = 0;
          while (isdigit(format[idx])) {
            places = places * 10 + (format[idx] - '0');
            idx++;
          }
          params.decimal_places = places;
          break;
        }
        case '+': {
          params.sign = true;
          idx++;
          break;
        }
        case 'f': {
          params.fixed = true;
          idx++;
          break;
        }
        case 'p': {
          params.exact_decimal_places = true;
          idx++;
          break;
        }
        default: {
          raise_exn(file, line, format, "Unexpected character in format");
        }
        }
      }
      if (format[idx] == 0) {
        raise_exn(file, line, format, "Format string ended unexpectedly");
      }
      idx++;
      return params;
    } else {
      output += format[idx];
      idx++;
    }
  }
  return std::nullopt;
}

} // namespace format_details
} // namespace bee
