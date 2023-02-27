#pragma once

#include "error.hpp"
#include "time.hpp"

#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace bee {

namespace fs = std::filesystem;

struct DirectoryContent {
  std::vector<fs::path> regular_files;
  std::vector<fs::path> directories;
};

struct FileSystem {
  static std::string read_stream(std::istream& stream);

  static bee::OrError<bee::Unit> mkdirs(const fs::path& path);

  static bee::OrError<bee::Unit> remove(const fs::path& path);

  static bee::OrError<bee::Unit> touch_file(const fs::path& filename);

  static bee::OrError<size_t> file_size(const fs::path& filename);

  static bee::OrError<bee::Unit> copy(const fs::path& from, const fs::path& to);

  static OrError<Time> file_mtime(const fs::path& filename);

  static bool exists(const fs::path& filename);

  static OrError<std::vector<fs::path>> list_regular_files(
    const fs::path& dir, bool recursive = false);

  static OrError<DirectoryContent> list_dir(const fs::path& dir);
};

} // namespace bee
