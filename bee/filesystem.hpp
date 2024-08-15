#pragma once

#include <istream>
#include <set>
#include <string>
#include <vector>

#include "file_path.hpp"
#include "or_error.hpp"
#include "time.hpp"

namespace bee {

struct DirectoryContent {
  std::vector<std::string> regular_files;
  std::vector<std::string> directories;
};

struct ListDirOptions {
  bool recursive = false;
  bool relative_path = false;
  std::set<std::string> exclude = {};
};

struct FileSystem {
  [[nodiscard]] static std::string read_stream(std::istream& stream);

  [[nodiscard]] static OrError<> mkdirs(const FilePath& path);

  [[nodiscard]] static OrError<> remove(const FilePath& path);
  [[nodiscard]] static OrError<> remove_all(const FilePath& path);

  [[nodiscard]] static OrError<> touch_file(const FilePath& filename);

  [[nodiscard]] static OrError<size_t> file_size(const FilePath& filename);

  [[nodiscard]] static OrError<> copy(const FilePath& from, const FilePath& to);

  [[nodiscard]] static OrError<Time> file_mtime(const FilePath& filename);

  [[nodiscard]] static bool exists(const FilePath& filename);
  [[nodiscard]] static bool is_directory(const FilePath& filename);
  [[nodiscard]] static bool is_regular_file(const FilePath& filename);

  [[nodiscard]] static OrError<std::vector<FilePath>> list_regular_files(
    const FilePath& dir, const ListDirOptions& opt = {});

  [[nodiscard]] static OrError<DirectoryContent> list_dir(const FilePath& dir);

  [[nodiscard]] static OrError<FilePath> create_temp_dir(
    const std::optional<FilePath>& prefix = {});

  [[nodiscard]] static bee::OrError<FilePath> absolute(const FilePath& path);
  [[nodiscard]] static bee::OrError<FilePath> canonical(const FilePath& path);

  [[nodiscard]] static bee::OrError<> create_symlink(
    const FilePath& path, const FilePath& target);

  [[nodiscard]] static bee::OrError<FilePath> current_dir();
  [[nodiscard]] static bee::OrError<> set_current_dir(const FilePath& path);
};

} // namespace bee
