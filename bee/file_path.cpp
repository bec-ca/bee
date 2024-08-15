#include "file_path.hpp"

using std::string;

namespace bee {

// Copy and move
FilePath::FilePath(const FilePath& other) = default;
FilePath::FilePath(FilePath&& other) noexcept = default;

FilePath& FilePath::operator=(const FilePath& other) = default;
FilePath& FilePath::operator=(FilePath&& other) noexcept = default;

// Ctors
FilePath::FilePath(const char* path) : _path(path) {}

FilePath::FilePath(const std::string& path) : _path(path) {}
FilePath::FilePath(std::string&& path) : _path(std::move(path)) {}

FilePath::FilePath(const fs::path& path) : _path(path) {}
FilePath::FilePath(fs::path&& path) : _path(std::move(path)) {}

FilePath::FilePath() {}

// Dtor
FilePath::~FilePath() noexcept {}

// accessors

std::string FilePath::to_string() const { return _path.string(); }
const fs::path& FilePath::to_std_path() const { return _path; }

string FilePath::extension() const { return _path.extension(); }
string FilePath::stem() const { return _path.stem(); }

FilePath FilePath::remove_extension() const
{
  auto copy = _path;
  copy.replace_extension("");
  return FilePath(std::move(copy));
}

std::string FilePath::filename() const { return _path.filename(); }
FilePath FilePath::parent() const { return FilePath(_path.parent_path()); }
bool FilePath::has_parent() const { return _path.has_parent_path(); }

const char* FilePath::data() const { return _path.c_str(); }

FilePath FilePath::relative_to(const FilePath& other) const
{
  return FilePath(_path.lexically_relative(other._path));
}

bool FilePath::is_parent_of(const FilePath& path_arg) const
{
  FilePath path = path_arg;
  while (true) {
    if (path == *this) { return true; }
    auto par = path.parent();
    if (par == path) { return false; }
    path = par;
  }
}

bool FilePath::is_child_of(const FilePath& other) const
{
  return other.is_parent_of(*this);
}

bool FilePath::empty() const { return _path.empty(); }

bool FilePath::is_absolute() const { return _path.is_absolute(); }

// operators

FilePath FilePath::operator/(const char* tail) const
{
  if (*tail == 0) { return *this; }
  return FilePath(_path / tail);
}

FilePath FilePath::operator/(const string& tail) const
{
  if (tail.empty()) { return *this; }
  return FilePath(_path / tail);
}

FilePath FilePath::operator/(const FilePath& tail) const
{
  if (tail._path.empty()) { return *this; }
  return FilePath(_path / tail._path);
}

FilePath& FilePath::operator/=(const char* tail)
{
  if (*tail != 0) { _path /= tail; }
  return *this;
}

FilePath& FilePath::operator/=(const string& tail)
{
  if (!tail.empty()) { _path /= tail; }
  return *this;
}

FilePath& FilePath::operator/=(const FilePath& tail)
{
  if (!tail.empty()) { _path /= tail._path; }
  return *this;
}

FilePath FilePath::operator+(const char* suffix) const
{
  return FilePath(_path.string() + suffix);
}

FilePath FilePath::operator+(const string& suffix) const
{
  return FilePath(_path.string() + suffix);
}

bool FilePath::operator<(const FilePath& other) const
{
  return _path < other._path;
}

} // namespace bee
