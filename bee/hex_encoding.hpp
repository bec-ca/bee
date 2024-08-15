#pragma once

#include <string>

#include "or_error.hpp"

namespace bee {

struct HexEncoding {
 public:
  static std::string to_hex(const std::string_view& str);

  static bee::OrError<std::string> of_hex(const std::string_view& str);
};
} // namespace bee
