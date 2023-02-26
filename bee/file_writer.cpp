#include "file_writer.hpp"

#include "file_descriptor.hpp"
#include <algorithm>
#include <cstdint>

using std::string;
using std::vector;

namespace bee {
namespace {

constexpr size_t max_to_buffer = 1 << 12;

bee::OrError<bee::Unit> write_to_fd(
  FileDescriptor& fd, const std::byte* data, size_t size)
{
  size_t bytes_writen = 0;
  while (bytes_writen < size) {
    bail(ret, fd.write(data + bytes_writen, size - bytes_writen));
    bytes_writen += ret;
  }
  return bee::ok();
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// FileWriter
//

bee::OrError<FileWriter::ptr> FileWriter::create(const string& filename)
{
  bail(fd, FileDescriptor::create_file(filename));
  return ptr(new FileWriter(std::move(fd)));
}

FileWriter::~FileWriter() { close(); }

void FileWriter::close()
{
  flush();
  _fd.close();
}

bee::OrError<bee::Unit> FileWriter::write(const std::byte* data, size_t size)
{
  if (size >= max_to_buffer) {
    bail_unit(flush());
    bail_unit(write_to_fd(_fd, reinterpret_cast<const std::byte*>(data), size));
  } else {
    _buffer.write(data, size);
    if (_buffer.size() >= max_to_buffer) { bail_unit(flush()); }
  }

  return bee::ok();
}

bee::OrError<bee::Unit> FileWriter::write(const DataBuffer& data)
{
  for (const auto& block : data) {
    bail_unit(write(block.data(), block.size()));
  }
  return bee::ok();
}

bee::OrError<bee::Unit> FileWriter::write(const string& data)
{
  return write(reinterpret_cast<const std::byte*>(data.data()), data.size());
}

bee::OrError<bee::Unit> FileWriter::write(const vector<std::byte>& data)
{
  return write(data.data(), data.size());
}

bee::OrError<bee::Unit> FileWriter::save_file(
  const string& filename, const string& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return bee::unit;
}

bee::OrError<bee::Unit> FileWriter::save_file(
  const string& filename, const vector<std::byte>& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return bee::unit;
}

FileWriter::FileWriter(FileDescriptor&& fd) : _fd(std::move(fd)) {}

bee::OrError<bee::Unit> FileWriter::flush()
{
  bail_unit(write_to_fd(_fd, _buffer.raw_data(), _buffer.size()));
  _buffer.clear();
  return bee::ok();
}

} // namespace bee
