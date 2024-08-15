#include "format.hpp"

#include <stdexcept>

#include "exn.hpp"

using std::string;

namespace bee {
namespace format_details {

void raise_exn(const Location& loc, const char* fmt, int idx, const char* msg)
{
  std::string message = std::string(msg) + ":\n| fmt: '" + fmt + "'\n|       ";
  for (int i = 0; i < idx; i++) { message += ' '; }
  message += '^';
  throw Exn(loc, std::move(message));
}

void write_one_rec(
  const Location& loc,
  const char* fmt,
  int idx,
  const FormatParams&,
  std::string&)
{
  raise_exn(loc, fmt, idx, "Extra formats in format string");
}

std::optional<FormatParams> get_next_format(
  const Location& loc, std::string& output, const char* fmt, int& idx)
{
  auto read_number = [&]() {
    int number = 0;
    while (isdigit(fmt[idx])) {
      number = number * 10 + (fmt[idx] - '0');
      idx++;
    }
    return number;
  };
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
        char c = fmt[idx];
        idx++;
        switch (c) {
        case ',': {
          params.comma = true;
        } break;
        case '.': {
          params.decimal_places = read_number();
        } break;
        case '+': {
          params.sign = true;
        } break;
        case 'f': {
          params.fixed = true;
        } break;
        case 'p': {
          params.exact_decimal_places = true;
        } break;
        case ' ': {
          params.left_pad_spaces = read_number();
        } break;
        case '0': {
          params.left_pad_zeroes = read_number();
        } break;
        case 'x': {
          params.hex = true;
        } break;
        default: {
          raise_exn(loc, fmt, idx - 1, "Unexpected character in format");
        }
        }
      }
      if (fmt[idx] == 0) {
        raise_exn(loc, fmt, idx, "Format string ended unexpectedly");
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
