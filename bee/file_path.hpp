#pragma once

#include <filesystem>

namespace bee {

namespace fs = std::filesystem;

struct FilePath {
 public:
  explicit FilePath(const std::string& path) : _path(path) {}
  explicit FilePath(std::string&& path) : _path(std::move(path)) {}

  FilePath(const FilePath& other);
  FilePath(FilePath&& other);

  FilePath();

  ~FilePath();

  // accessors

  std::string to_string() const;

  const fs::path& to_std_path() const;

  std::string filename() const;
  FilePath parent() const;

  std::string extension() const;
  std::string stem() const;

  FilePath relative_to(const FilePath& path) const;

  const char* data() const;

  bool is_parent_of(const FilePath& path) const;
  bool is_child_of(const FilePath& path) const;

  // static constructors

  static FilePath of_string(const std::string& path);
  static FilePath of_std_path(const fs::path& path);

  static FilePath of_string(std::string&& path);
  static FilePath of_std_path(fs::path&& path);

  // operators

  FilePath operator/(const std::string& tail) const;
  FilePath operator/(const FilePath& tail) const;

  FilePath& operator/=(const std::string& tail);

  FilePath operator+(const std::string& suffix) const;

  FilePath& operator=(const FilePath& other);
  FilePath& operator=(FilePath&& other);

  bool operator<(const FilePath& other) const;

  std::strong_ordering operator<=>(const FilePath& other) const = default;

 private:
  fs::path _path;
};

} // namespace bee
