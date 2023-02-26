#include "file_reader.hpp"

#include "file_descriptor.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::vector;

namespace bee {

namespace {

template <class T>
void append_bytes(T& output, const std::byte* begin, const std::byte* end)
{
  auto initial_size = output.size();
  auto bytes = end - begin;
  output.resize(initial_size + bytes);
  memcpy(&output[initial_size], begin, bytes);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// FileReader
//

bee::OrError<FileReader::ptr> FileReader::open(const string& filename)
{
  bail(fd, FileDescriptor::open_file(filename));

  return ptr(new FileReader(std::move(fd)));
}

FileReader::~FileReader() {}

bee::OrError<size_t> FileReader::read(std::byte* buffer, size_t size)
{
  size_t total_read = 0;

  auto remaining = [&]() { return size - total_read; };

  total_read += _read_from_buffer(buffer, size);
  if (remaining() == 0) {
    // Nothing else to do there
  } else if (remaining() < sizeof(_buffer)) {
    _maybe_read_more();
    total_read += _read_from_buffer(buffer + total_read, size - total_read);
  } else {
    total_read += _read_full(buffer + total_read, size - total_read);
  }

  if (total_read == 0 && _last_error.has_value()) { return *_last_error; }

  return total_read;
}

bee::OrError<string> FileReader::read_str(size_t size)
{
  string output;
  output.resize(size);
  bail(bytes_read, read(reinterpret_cast<std::byte*>(output.data()), size));
  output.resize(bytes_read);
  return output;
}

bee::OrError<string> FileReader::read_line()
{
  string output;
  bool found_eol = false;
  while (_maybe_read_more()) {
    char c = std::to_integer<char>(_buffer[_buffer_pos++]);
    if (c == '\n') {
      found_eol = true;
      break;
    }
    output += c;
  }
  if (!found_eol && output.empty() && _last_error.has_value()) {
    return *_last_error;
  }
  return output;
}

bee::OrError<vector<string>> FileReader::read_all_lines()
{
  vector<string> output;
  while (!is_eof()) {
    bail(line, read_line());
    output.push_back(line);
  }
  return output;
}

bee::OrError<string> FileReader::read_all()
{
  string output;
  while (_maybe_read_more()) {
    append_bytes(output, buffer_begin(), buffer_end());
    clear_buffer();
  }
  return output;
}

bee::OrError<vector<std::byte>> FileReader::read_all_bytes()
{
  vector<std::byte> output;
  while (_maybe_read_more()) {
    append_bytes(output, buffer_begin(), buffer_end());
    clear_buffer();
  }
  return output;
}

bee::OrError<char> FileReader::read_char()
{
  if (!_maybe_read_more()) {
    if (_last_error.has_value()) {
      return *_last_error;
    } else if (is_eof()) {
      return bee::Error("EOF");
    } else {
      return bee::Error("Unexpected error");
    }
  }
  return std::to_integer<char>(_buffer[_buffer_pos++]);
}

bool FileReader::is_eof() { return !_maybe_read_more(); }

FileReader::FileReader(FileDescriptor&& fd) : _fd(std::move(fd).to_unique()) {}

size_t FileReader::_available_on_buffer() const
{
  return _buffer_size - _buffer_pos;
}

bool FileReader::_buffer_has_data() const { return _available_on_buffer() > 0; }

size_t FileReader::_read_into(std::byte* buffer, size_t size)
{
  if (_eof || _last_error.has_value()) return 0;
  auto res = _fd->read(buffer, size);
  if (res.is_error()) {
    _last_error = std::move(res.error());
    return 0;
  }
  auto ret = res.value();
  if (ret.is_eof()) { _eof = true; }
  return ret.bytes_read();
}

size_t FileReader::_read_full(std::byte* buffer, size_t size)
{
  size_t bytes_read = 0;
  while (bytes_read < size) {
    auto ret = _read_into(buffer + bytes_read, size - bytes_read);
    if (ret == 0) break;
    bytes_read += ret;
  }
  return bytes_read;
}

size_t FileReader::_read_from_buffer(std::byte* buffer, size_t size)
{
  size_t to_copy = std::min(size, _available_on_buffer());
  memcpy(buffer, _buffer + _buffer_pos, to_copy);
  _buffer_pos += to_copy;
  return to_copy;
}

bool FileReader::_maybe_read_more()
{
  if (_buffer_has_data()) return true;
  auto ret = _read_full(_buffer, sizeof(_buffer));
  if (ret == 0) return false;
  _buffer_pos = 0;
  _buffer_size = ret;
  return true;
}

bee::OrError<string> FileReader::read_file(const string& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all();
}

bee::OrError<vector<std::byte>> FileReader::read_file_bytes(
  const string& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all_bytes();
}

void FileReader::close() { _fd->close(); }

const std::byte* FileReader::buffer_begin() const
{
  return _buffer + _buffer_pos;
}

const std::byte* FileReader::buffer_end() const
{
  return buffer_begin() + _available_on_buffer();
}

void FileReader::clear_buffer() { _buffer_pos = (_buffer_size = 0); }

bee::OrError<size_t> FileReader::remaining_bytes()
{
  return _fd->remaining_bytes();
}

} // namespace bee
