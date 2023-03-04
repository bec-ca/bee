#include "filesystem.hpp"

#include "bee/file_path.hpp"
#include "file_reader.hpp"
#include "file_writer.hpp"
#include "format_filesystem.hpp"
#include "util.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>

using std::error_code;
using std::istream;
using std::ofstream;
using std::string;
using std::vector;

namespace bee {

string FileSystem::read_stream(istream& stream)
{
  string contents;
  while (stream.good()) {
    int offset = contents.size();
    int buffer_size = 1 << 16;
    contents.resize(offset + buffer_size);
    stream.read(contents.data() + offset, buffer_size);
    int read = stream.gcount();
    if (read < buffer_size) { contents.resize(offset + read); }
  }
  return contents;
}

OrError<Unit> FileSystem::mkdirs(const FilePath& path)
{
  error_code ec;
  fs::create_directories(path.to_std_path(), ec);
  if (ec) {
    return Error::format(
      "Failed to create directory '$': $", path, ec.message());
  }
  return unit;
}

OrError<Unit> FileSystem::remove(const FilePath& path)
{
  error_code ec;
  fs::remove(path.to_std_path(), ec);
  if (ec) {
    return Error::format("Failed to cremove file '$': $", path, ec.message());
  }
  return unit;
}

OrError<Unit> FileSystem::touch_file(const FilePath& filename)
{
  ofstream output(filename.to_std_path());
  if (!output.good()) {
    return Error::format("Failed to touch file $", filename);
  }
  return unit;
}

OrError<Unit> FileSystem::copy(const FilePath& from, const FilePath& to)
{
  error_code ec;
  fs::copy(
    from.to_std_path(),
    to.to_std_path(),
    fs::copy_options::overwrite_existing,
    ec);
  if (ec) {
    return Error::format("Failed to copy file: $", ec.message());
  } else {
    return unit;
  }
}

OrError<size_t> FileSystem::file_size(const FilePath& filename)
{
  error_code ec;
  auto size = fs::file_size(filename.to_std_path(), ec);
  if (ec) {
    return Error::format(
      "Failed to check file size '$': $", filename, ec.message());
  }
  return size;
}

OrError<Time> FileSystem::file_mtime(const FilePath& filename)
{
  error_code ec;
  auto mtime = fs::last_write_time(filename.to_std_path(), ec);
  if (ec) {
    return Error::format(
      "Failed to check file mtime '$': $", filename, ec.message());
  }
  return Time::of_nanos_since_epoch(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      mtime.time_since_epoch())
      .count());
}

bool FileSystem::exists(const FilePath& filename)
{
  return fs::exists(filename.to_std_path());
}

OrError<vector<FilePath>> FileSystem::list_regular_files(
  const FilePath& dir, bool recursive)
{
  error_code ec;
  vector<FilePath> output;
  for (const auto& p : fs::directory_iterator(dir.to_std_path(), ec)) {
    auto path = FilePath::of_std_path(p.path());
    if (p.is_directory()) {
      if (recursive) {
        bail(sub_files, list_regular_files(path, true));
        concat(output, sub_files);
      }
    } else if (p.is_regular_file()) {
      output.push_back(path);
    }
  }
  if (ec) {
    return Error::format("Failed to list directory '$': $", dir, ec.message());
  }
  return output;
}

OrError<DirectoryContent> FileSystem::list_dir(const FilePath& dir)
{
  error_code ec;
  DirectoryContent output;
  for (const auto& p : fs::directory_iterator(dir.to_std_path(), ec)) {
    auto path = FilePath::of_std_path(p.path());
    if (p.is_regular_file()) {
      output.regular_files.push_back(path);
    } else if (p.is_directory()) {
      output.directories.push_back(path);
    }
  }
  if (ec) {
    return Error::format("Failed to list directory '$': $", dir, ec.message());
  }
  return output;
}

} // namespace bee
