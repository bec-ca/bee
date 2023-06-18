#include "format.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "exn.hpp"

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

void raise_exn(const Location& loc, const char* fmt, const char* msg)
{
  throw Exn(loc, std::string(msg) + ": '" + fmt + "'");
}

void write_one_rec(
  const Location& loc, const char* fmt, int, const FormatParams&, std::string&)
{
  raise_exn(loc, fmt, "Extra formats in format string");
}

std::optional<FormatParams> get_next_format(
  const Location& loc, std::string& output, const char* fmt, int& idx)
{
  while (fmt[idx]) {
    FormatParams params;
    if ((fmt[idx] == '$' && fmt[idx + 1] != '$')) {
      idx++;
      return FormatParams();
    } else if (fmt[idx] == '$' && fmt[idx + 1] == '$') {
      output += '$';
      idx += 2;
    } else if (fmt[idx] == '{' && fmt[idx + 1] == '{') {
      output += '{';
      idx += 2;
    } else if (fmt[idx] == '{') {
      idx++;
      while (fmt[idx] && fmt[idx] != '}') {
        switch (fmt[idx]) {
        case ',': {
          params.comma = true;
          idx++;
          break;
        }
        case '.': {
          idx++;
          int places = 0;
          while (isdigit(fmt[idx])) {
            places = places * 10 + (fmt[idx] - '0');
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
          raise_exn(loc, fmt, "Unexpected character in format");
        }
        }
      }
      if (fmt[idx] == 0) {
        raise_exn(loc, fmt, "Format string ended unexpectedly");
      }
      idx++;
      return params;
    } else {
      output += fmt[idx];
      idx++;
    }
  }
  return std::nullopt;
}

} // namespace format_details
} // namespace bee
