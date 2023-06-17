#pragma once

#include <string>

#include "format_params.hpp"
#include "to_string_t.hpp"

namespace bee {

std::string float_to_string(double number);

template <> struct to_string_t<float> {
  static std::string convert(float value, const FormatParams& p);
};

template <> struct to_string_t<double> {
  static std::string convert(double value, const FormatParams& p);
};

} // namespace bee
