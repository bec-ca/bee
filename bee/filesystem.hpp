#pragma once

#include "error.hpp"
#include "time.hpp"

#include "file_path.hpp"

#include <filesystem>
#include <istream>
#include <set>
#include <string>
#include <vector>

namespace bee {

// TODO: add temp_directory_path, current_path

struct DirectoryContent {
  std::vector<FilePath> regular_files;
  std::vector<FilePath> directories;
};

struct ListDirOptions {
  bool recursive = false;
  bool relative_path = false;
  std::set<std::string> exclude = {};
};

struct FileSystem {
  static std::string read_stream(std::istream& stream);

  static OrError<Unit> mkdirs(const FilePath& path);

  static OrError<Unit> remove(const FilePath& path);
  static OrError<Unit> remove_all(const FilePath& path);

  static OrError<Unit> touch_file(const FilePath& filename);

  static OrError<size_t> file_size(const FilePath& filename);

  static OrError<Unit> copy(const FilePath& from, const FilePath& to);

  static OrError<Time> file_mtime(const FilePath& filename);

  static bool exists(const FilePath& filename);

  static OrError<std::vector<FilePath>> list_regular_files(
    const FilePath& dir, const ListDirOptions& opt = {});

  static OrError<DirectoryContent> list_dir(const FilePath& dir);
};

} // namespace bee
