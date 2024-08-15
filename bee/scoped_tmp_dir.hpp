#pragma once

#include "file_path.hpp"
#include "or_error.hpp"

namespace bee {

struct ScopedTmpDir {
 public:
  static OrError<ScopedTmpDir> create();

  ScopedTmpDir(const ScopedTmpDir& other) = delete;
  ScopedTmpDir(ScopedTmpDir&& other) noexcept;

  ScopedTmpDir& operator=(ScopedTmpDir&& other);

  ~ScopedTmpDir() noexcept;

  const FilePath& path() const { return *_path; }

 private:
  explicit ScopedTmpDir(const FilePath& path);

  std::optional<FilePath> _path;
};

} // namespace bee
