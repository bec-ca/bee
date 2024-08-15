#pragma once

namespace bee {

struct FormatParams {
  bool comma = false;
  int decimal_places = 6;
  bool sign = false;
  bool fixed = false;
  bool exact_decimal_places = false;
  bool hex = false;
  int left_pad_zeroes = 0;
  int left_pad_spaces = 0;
};

} // namespace bee
