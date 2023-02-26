#pragma once

#include "error.hpp"
#include "time.hpp"

#include <string>

namespace bee {

struct Sys {
  static bee::OrError<bee::Time> file_mtime(const std::string& filename);
};

} // namespace bee
