#include "file_path.hpp"

using std::string;

namespace bee {

FilePath::FilePath() {}
FilePath::~FilePath() {}

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

std::string FilePath::to_string() const { return _path.string(); }
const fs::path& FilePath::to_std_path() const { return _path; }

FilePath FilePath::operator/(const string& tail) const
{
  return FilePath(_path / tail);
}

FilePath FilePath::operator/(const FilePath& tail) const
{
  return FilePath(_path / tail._path);
}

string FilePath::extension() const { return _path.extension(); }
string FilePath::stem() const { return _path.stem(); }

std::string FilePath::filename() const { return _path.filename(); }
FilePath FilePath::parent_path() const { return FilePath(_path.parent_path()); }

bool FilePath::operator<(const FilePath& other) const
{
  return _path < other._path;
}

} // namespace bee
