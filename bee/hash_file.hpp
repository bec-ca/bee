#pragma once

#include <string>

#include "file_path.hpp"
#include "or_error.hpp"

namespace bee {

struct HashFile {
 public:
  static OrError<std::string> compute_simple_hash(const FilePath& filename);
};

} // namespace bee
