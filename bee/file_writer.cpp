#include "file_writer.hpp"

#include <algorithm>
#include <cstdint>

#include "fd.hpp"

using std::string;
using std::vector;

namespace bee {
namespace {

constexpr size_t max_to_buffer = 1 << 12;

OrError<> write_to_fd(FD& fd, const std::byte* data, size_t size)
{
  size_t bytes_writen = 0;
  while (bytes_writen < size) {
    bail(ret, fd.write(data + bytes_writen, size - bytes_writen));
    bytes_writen += ret;
  }
  return ok();
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// FileWriter
//

OrError<FileWriter::ptr> FileWriter::create(const FilePath& filename)
{
  bail(fd, FD::create_file(filename));
  return ptr(new FileWriter(std::move(fd)));
}

FileWriter::~FileWriter() { close(); }

void FileWriter::close()
{
  flush();
  _fd.close();
}

OrError<> FileWriter::write(const std::byte* data, size_t size)
{
  if (size >= max_to_buffer) {
    bail_unit(flush());
    bail_unit(write_to_fd(_fd, reinterpret_cast<const std::byte*>(data), size));
  } else {
    _buffer.write(data, size);
    if (_buffer.size() >= max_to_buffer) { bail_unit(flush()); }
  }

  return ok();
}

OrError<> FileWriter::write(const DataBuffer& data)
{
  for (const auto& block : data) {
    bail_unit(write(block.data(), block.size()));
  }
  return ok();
}

OrError<> FileWriter::write(const string& data)
{
  return write(reinterpret_cast<const std::byte*>(data.data()), data.size());
}

OrError<> FileWriter::write(const vector<std::byte>& data)
{
  return write(data.data(), data.size());
}

OrError<> FileWriter::save_file(const FilePath& filename, const string& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return ok();
}

OrError<> FileWriter::save_file(
  const FilePath& filename, const vector<std::byte>& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return ok();
}

FileWriter::FileWriter(FD&& fd) : _fd(std::move(fd)) {}

OrError<> FileWriter::flush()
{
  bail_unit(write_to_fd(_fd, _buffer.raw_data(), _buffer.size()));
  _buffer.clear();
  return ok();
}

} // namespace bee
