#include "file_path.hpp"

using std::string;

namespace bee {

FilePath::FilePath(const FilePath& other) = default;
FilePath::FilePath(FilePath&& other) = default;

FilePath::FilePath() {}
FilePath::~FilePath() {}

// accessors

std::string FilePath::to_string() const { return _path.string(); }
const fs::path& FilePath::to_std_path() const { return _path; }

string FilePath::extension() const { return _path.extension(); }
string FilePath::stem() const { return _path.stem(); }

std::string FilePath::filename() const { return _path.filename(); }
FilePath FilePath::parent() const { return FilePath(_path.parent_path()); }

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

// static constructors

FilePath FilePath::of_string(const std::string& path) { return FilePath(path); }
FilePath FilePath::of_std_path(const fs::path& path) { return FilePath(path); }

FilePath FilePath::of_string(std::string&& path)
{
  return FilePath(std::move(path));
}
FilePath FilePath::of_std_path(fs::path&& path)
{
  return FilePath(std::move(path));
}

// operators

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

FilePath FilePath::operator+(const string& suffix) const
{
  return FilePath(_path.string() + suffix);
}

bool FilePath::operator<(const FilePath& other) const
{
  return _path < other._path;
}

FilePath& FilePath::operator=(const FilePath& other) = default;
FilePath& FilePath::operator=(FilePath&& other) = default;

} // namespace bee
