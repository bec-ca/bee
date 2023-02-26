#pragma once

#include <string>

namespace bee {

struct PrettyPrint {
 public:
  static std::string format_double(double value, int decimals);
};

} // namespace bee
