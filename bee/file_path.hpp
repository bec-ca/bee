#pragma once

#include <filesystem>

namespace bee {

namespace fs = std::filesystem;

struct FilePath {
 public:
  FilePath();
  ~FilePath();

  static FilePath of_string(const std::string& path);
  static FilePath of_std_path(const fs::path& path);

  static FilePath of_string(std::string&& path);
  static FilePath of_std_path(fs::path&& path);

  std::string to_string() const;

  const fs::path& to_std_path() const;

  FilePath operator/(const std::string& tail) const;
  FilePath operator/(const FilePath& tail) const;

  std::string filename() const;
  FilePath parent_path() const;

  std::string extension() const;
  std::string stem() const;

  bool operator<(const FilePath& other) const;

  std::strong_ordering operator<=>(const FilePath& other) const = default;

 private:
  explicit FilePath(const std::string& path) : _path(path) {}
  explicit FilePath(const fs::path& path) : _path(path) {}
  explicit FilePath(std::string&& path) : _path(std::move(path)) {}
  explicit FilePath(fs::path&& path) : _path(std::move(path)) {}

  fs::path _path;
};

} // namespace bee
