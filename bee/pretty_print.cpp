#include "pretty_print.hpp"

#include <cmath>

using std::string;

namespace bee {

string PrettyPrint::format_double(double value, int decimals)
{
  bool negative = false;
  if (value < 0) {
    negative = true;
    value = -value;
  }
  double rounding = 0.5;
  for (int i = 0; i < decimals; i++) { rounding /= 10.0; }
  value += rounding;
  int num_digits = 1;
  while (value >= std::pow(10.0, num_digits)) { num_digits++; }
  string output;
  double done = 0.0;
  auto put_one_digit = [&]() {
    double base = std::pow(10.0, num_digits - 1);
    int digit = std::trunc((value - done) / base) + 0.5;
    output += digit + '0';
    done += digit * base;
    num_digits--;
  };
  while (num_digits > 0) { put_one_digit(); }
  if (decimals > 0) {
    output += ".";
    for (int i = 0; i < decimals; i++) { put_one_digit(); }
    while (output.back() == '0') { output.pop_back(); }
    if (output.back() == '.') output.pop_back();
  }

  if (negative) { output = "-" + output; }

  return output;
}

} // namespace bee
