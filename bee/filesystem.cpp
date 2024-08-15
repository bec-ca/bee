#include "filesystem.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <queue>
#include <ratio>

#include <unistd.h>

#include "errno_msg.hpp"
#include "file_path.hpp"
#include "file_reader.hpp"
#include "file_writer.hpp"
#include "format_filesystem.hpp"
#include "util.hpp"

using std::error_code;
using std::istream;
using std::ofstream;
using std::string;
using std::vector;

namespace bee {
namespace {

OrError<vector<FilePath>> list_regular_files_impl(
  const FilePath& base_dir, const ListDirOptions& opts)
{
  vector<FilePath> output;
  std::queue<FilePath> queue;
  queue.push(FilePath(""));
  while (!queue.empty()) {
    auto dir = queue.front();
    queue.pop();
    error_code ec;
    auto entries = fs::directory_iterator((base_dir / dir).to_std_path(), ec);
    if (ec) {
      return Error::fmt("Failed to list directory '$': $", dir, ec.message());
    }
    for (const auto& p : entries) {
      auto path = FilePath(p.path());
      auto filename = path.filename();
      auto rel_path = dir / filename;
      if (opts.exclude.contains(filename)) { continue; }
      if (p.is_directory()) {
        if (opts.recursive) { queue.push(rel_path); }
      } else if (p.is_regular_file()) {
        if (opts.relative_path) {
          output.push_back(rel_path);
        } else {
          output.push_back(path);
        }
      }
    }
  }
  return output;
}

} // namespace

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

OrError<> FileSystem::mkdirs(const FilePath& path)
{
  error_code ec;
  fs::create_directories(path.to_std_path(), ec);
  if (ec) {
    return Error::fmt("Failed to create directory '$': $", path, ec.message());
  }
  return ok();
}

OrError<> FileSystem::remove(const FilePath& path)
{
  error_code ec;
  fs::remove(path.to_std_path(), ec);
  if (ec) {
    return Error::fmt("Failed to remove file '$': $", path, ec.message());
  }
  return ok();
}

OrError<> FileSystem::remove_all(const FilePath& path)
{
  error_code ec;
  fs::remove_all(path.to_std_path(), ec);
  if (ec) {
    return Error::fmt("Failed to remove all files '$': $", path, ec.message());
  }
  return ok();
}

OrError<> FileSystem::touch_file(const FilePath& filename)
{
  ofstream output(filename.to_std_path());
  if (!output.good()) { return Error::fmt("Failed to touch file $", filename); }
  return ok();
}

OrError<> FileSystem::copy(const FilePath& from, const FilePath& to)
{
  error_code ec;
  fs::copy(
    from.to_std_path(),
    to.to_std_path(),
    fs::copy_options::overwrite_existing,
    ec);
  if (ec) {
    return Error::fmt(
      "Failed to copy file '$' to '$': $", from, to, ec.message());
  } else {
    return ok();
  }
}

OrError<size_t> FileSystem::file_size(const FilePath& filename)
{
  error_code ec;
  auto size = fs::file_size(filename.to_std_path(), ec);
  if (ec) {
    return Error::fmt(
      "Failed to check file size '$': $", filename, ec.message());
  }
  return size;
}

OrError<Time> FileSystem::file_mtime(const FilePath& filename)
{
  error_code ec;
  auto mtime = std::chrono::file_clock::to_sys(
                 fs::last_write_time(filename.to_std_path(), ec))
                 .time_since_epoch();
  if (ec) {
    return Error::fmt(
      "Failed to check file mtime '$': $", filename, ec.message());
  }

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(mtime);
  return Time::of_span_since_epoch(Span::of_nanos(duration.count()));
}

bool FileSystem::exists(const FilePath& filename)
{
  error_code ec;
  std::ignore = fs::symlink_status(filename.to_std_path(), ec);
  return !ec;
}

bool FileSystem::is_directory(const FilePath& filename)
{
  return fs::is_directory(filename.to_std_path());
}

bool FileSystem::is_regular_file(const FilePath& filename)
{
  return fs::is_regular_file(filename.to_std_path());
}

OrError<vector<FilePath>> FileSystem::list_regular_files(
  const FilePath& base_dir, const ListDirOptions& opts)
{
  return list_regular_files_impl(base_dir, opts);
}

OrError<DirectoryContent> FileSystem::list_dir(const FilePath& dir)
{
  error_code ec;
  DirectoryContent output;
  for (const auto& p : fs::directory_iterator(dir.to_std_path(), ec)) {
    auto path = FilePath(p.path());
    auto filename = path.filename();
    if (p.is_regular_file()) {
      output.regular_files.push_back(filename);
    } else if (p.is_directory()) {
      output.directories.push_back(filename);
    }
  }
  if (ec) {
    return Error::fmt("Failed to list directory '$': $", dir, ec.message());
  }
  return output;
}

OrError<FilePath> FileSystem::create_temp_dir(
  const std::optional<FilePath>& prefix)
{
  std::string str;
  if (prefix.has_value()) {
    str = prefix->to_string();
  } else {
    str = "/tmp/bee-";
  }
  str += "XXXXXX";
  if (mkdtemp(&str[0]) == nullptr) {
    return EF("Failed to create temp dir: $", errno_msg());
  }
  return FilePath(str);
}

bee::OrError<FilePath> FileSystem::absolute(const FilePath& path)
{
  error_code ec;
  auto out = fs::absolute(path.to_std_path(), ec);
  if (ec) { return EF("Failed to determine absolute path: $", path); }
  return FilePath(out);
}

bee::OrError<FilePath> FileSystem::canonical(const FilePath& path)
{
  error_code ec;
  auto out = fs::canonical(path.to_std_path(), ec);
  if (ec) { return EF("Failed to determine canonical path: $", path); }
  return FilePath(out);
}

bee::OrError<> FileSystem::create_symlink(
  const FilePath& path, const FilePath& target)
{
  error_code ec;
  std::filesystem::create_symlink(path.to_std_path(), target.to_std_path(), ec);
  if (ec) { return EF("Failed to create symlink: $", ec.message()); }
  return bee::ok();
}

bee::OrError<FilePath> FileSystem::current_dir()
{
  error_code ec;
  auto out = std::filesystem::current_path(ec);
  if (ec) { return EF("Failed to get current path: $", ec.message()); }
  return FilePath(out);
}

bee::OrError<> FileSystem::set_current_dir(const FilePath& dir)
{
  error_code ec;
  std::filesystem::current_path(dir.to_std_path(), ec);
  if (ec) { return EF("Failed to set current path: $", ec.message()); }
  return bee::ok();
}

} // namespace bee
