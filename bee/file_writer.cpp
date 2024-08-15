#include "file_writer.hpp"

#include <algorithm>
#include <cstdint>

#include "fd.hpp"

using std::string;
using std::vector;

namespace bee {
namespace {

constexpr size_t max_to_buffer = 1 << 12;

OrError<size_t> write_to_fd(FD& fd, const std::byte* data, size_t size)
{
  size_t bytes_writen = 0;
  while (bytes_writen < size) {
    bail(ret, fd.write(data + bytes_writen, size - bytes_writen));
    bytes_writen += ret;
  }
  return bytes_writen;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// FileWriter
//

FileWriter::FileWriter(FD&& fd, bool buffered)
    : FileWriter(std::make_shared<FD>(std::move(fd)), buffered)
{}

FileWriter::FileWriter(const FD::shared_ptr& fd, bool buffered)
    : _fd(fd), _is_tty(_fd->is_tty()), _buffered(buffered)
{}

OrError<FileWriter::ptr> FileWriter::create(const FilePath& filename)
{
  bail(fd, FD::create_file(filename));
  return ptr(new FileWriter(std::move(fd), true));
}

FileWriter::~FileWriter() noexcept { close(); }

bool FileWriter::close()
{
  std::ignore = flush();
  return _fd->close();
}

OrError<size_t> FileWriter::write_raw(const std::byte* data, size_t size)
{
  if (size >= max_to_buffer) {
    bail_unit(flush());
    return write_to_fd(*_fd, data, size);
  } else {
    _buffer.write(data, size);
    if (_buffer.size() >= max_to_buffer || !_buffered) { bail_unit(flush()); }
    return size;
  }
}

OrError<> FileWriter::write_file(
  const FilePath& filename, const string& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return ok();
}

OrError<> FileWriter::write_file(
  const FilePath& filename, const vector<std::byte>& content)
{
  bail(file, FileWriter::create(filename));
  bail_unit(file->write(content));
  return ok();
}

OrError<> FileWriter::flush()
{
  bail_unit(write_to_fd(*_fd, _buffer.raw_data(), _buffer.size()));
  _buffer.clear();
  return ok();
}

bool FileWriter::is_tty() const { return _is_tty; }

FileWriter& FileWriter::stderr()
{
  static auto file = std::make_unique<FileWriter>(FD::stderr_filedesc(), false);
  return *file;
}

FileWriter& FileWriter::stdout()
{
  static auto file = std::make_unique<FileWriter>(FD::stdout_filedesc(), true);
  return *file;
}

void FileWriter::set_buffered(bool buffered) { _buffered = buffered; }

} // namespace bee
