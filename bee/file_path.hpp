#pragma once

#include <filesystem>

namespace bee {

namespace fs = std::filesystem;

struct FilePath {
 public:
  FilePath(const FilePath& other);
  FilePath(FilePath&& other) noexcept;

  explicit FilePath(const char* path);

  explicit FilePath(const std::string& path);
  explicit FilePath(std::string&& path);

  explicit FilePath(const fs::path& path);
  explicit FilePath(fs::path&& path);

  FilePath();

  ~FilePath() noexcept;

  // accessors

  std::string to_string() const;

  const fs::path& to_std_path() const;

  std::string filename() const;
  FilePath parent() const;
  bool has_parent() const;

  // Returns the file extension if any, including the ".". Returns an empty
  // string if there are no extension.
  // Eg: "dir/foobar.txt" -> ".txt"
  std::string extension() const;
  std::string stem() const;

  FilePath remove_extension() const;

  FilePath relative_to(const FilePath& path) const;

  const char* data() const;

  bool is_parent_of(const FilePath& path) const;
  bool is_child_of(const FilePath& path) const;

  bool empty() const;

  bool is_absolute() const;

  // operators

  FilePath operator/(const char* tail) const;
  FilePath operator/(const std::string& tail) const;
  FilePath operator/(const FilePath& tail) const;

  FilePath& operator/=(const char* tail);
  FilePath& operator/=(const std::string& tail);
  FilePath& operator/=(const FilePath& tail);

  FilePath operator+(const char* suffix) const;
  FilePath operator+(const std::string& suffix) const;

  FilePath& operator=(const FilePath& other);
  FilePath& operator=(FilePath&& other) noexcept;

  bool operator<(const FilePath& other) const;

  std::strong_ordering operator<=>(const FilePath& other) const = default;

 private:
  fs::path _path;
};

} // namespace bee
