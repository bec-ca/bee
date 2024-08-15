#include "scoped_tmp_dir.hpp"

#include "bee/filesystem.hpp"

namespace bee {

ScopedTmpDir::ScopedTmpDir(const FilePath& path) : _path(path) {}

ScopedTmpDir::ScopedTmpDir(ScopedTmpDir&& other) noexcept
    : _path(std::move(other._path))
{
  other._path.reset();
}

ScopedTmpDir& ScopedTmpDir::operator=(ScopedTmpDir&& other)
{
  if (_path.has_value()) { must_unit(FileSystem::remove_all(*_path)); }
  _path = std::move(other._path);
  other._path.reset();
  return *this;
}

OrError<ScopedTmpDir> ScopedTmpDir::create()
{
  bail(path, FileSystem::create_temp_dir());
  return ScopedTmpDir(path);
}

ScopedTmpDir::~ScopedTmpDir() noexcept
{
  if (_path.has_value()) { must_unit(FileSystem::remove_all(*_path)); }
}

} // namespace bee
