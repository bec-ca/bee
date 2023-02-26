#include "filesystem.hpp"

#include "file_reader.hpp"
#include "file_writer.hpp"

#include <filesystem>
#include <fstream>

using std::error_code;
using std::istream;
using std::ofstream;
using std::string;

namespace fs = std::filesystem;

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

bee::OrError<bee::Unit> FileSystem::mkdirs(const string& path)
{
  error_code ec;
  fs::create_directories(path, ec);
  if (ec) {
    return bee::Error::format(
      "Failed to create directory '$': $", path, ec.message());
  }
  return bee::unit;
}

bee::OrError<bee::Unit> FileSystem::remove(const string& path)
{
  error_code ec;
  fs::remove(path, ec);
  if (ec) {
    return bee::Error::format(
      "Failed to cremove file '$': $", path, ec.message());
  }
  return bee::unit;
}

bee::OrError<bee::Unit> FileSystem::touch_file(const string& filename)
{
  ofstream output(filename);
  if (!output.good()) {
    return bee::Error::format("Failed to touch file $", filename);
  }
  return bee::unit;
}

bee::OrError<bee::Unit> FileSystem::copy(const string& from, const string& to)
{
  error_code ec;
  fs::copy(from, to, fs::copy_options::overwrite_existing, ec);
  if (ec) {
    return bee::Error::format("Failed to copy file: $", ec.message());
  } else {
    return bee::unit;
  }
}

bee::OrError<size_t> FileSystem::file_size(const string& filename)
{
  error_code ec;
  auto size = fs::file_size(filename, ec);
  if (ec) {
    return bee::Error::format(
      "Failed to check file size '$': $", filename, ec.message());
  }
  return size;
}

} // namespace bee
