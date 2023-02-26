#pragma once

#include "error.hpp"

#include <istream>
#include <string>

namespace bee {

struct FileSystem {
  static std::string read_stream(std::istream& stream);

  static bee::OrError<bee::Unit> mkdirs(const std::string& path);

  static bee::OrError<bee::Unit> remove(const std::string& path);

  static bee::OrError<bee::Unit> touch_file(const std::string& filename);

  static bee::OrError<size_t> file_size(const std::string& filename);

  static bee::OrError<bee::Unit> copy(
    const std::string& from, const std::string& to);
};

} // namespace bee
