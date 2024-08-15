#include "file_reader.hpp"

#include <cstring>

#include "fd.hpp"

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

OrError<FileReader::ptr> FileReader::open(const FilePath& filename)
{
  bail(fd, FD::open_file(filename));
  return std::make_unique<FileReader>(std::move(fd).to_shared());
}

FileReader::ptr FileReader::from_fd(const FD::shared_ptr& fd)
{
  return std::make_unique<FileReader>(fd);
}

FileReader::~FileReader() noexcept {}

OrError<size_t> FileReader::read_raw(std::byte* buffer, size_t size)
{
  size_t total_read = 0;

  auto remaining = [&]() { return size - total_read; };

  total_read += _read_from_buffer(buffer, size);
  if (remaining() == 0) {
    // Nothing else to do there
  } else if (remaining() < BufferSize) {
    _maybe_read_more();
    total_read += _read_from_buffer(buffer + total_read, size - total_read);
  } else {
    total_read += _read_full(buffer + total_read, size - total_read);
  }

  if (total_read == 0 && _last_error.has_value()) { return *_last_error; }

  return total_read;
}

OrError<std::optional<string>> FileReader::read_line()
{
  string output;
  bool found_eol = false;
  while (_maybe_read_more()) {
    char c = static_cast<char>(_buffer[_buffer_pos++]);
    if (c == '\n') {
      found_eol = true;
      break;
    } else if (c == '\r') {
      continue;
    }
    output += c;
  }
  if (!found_eol && output.empty()) {
    if (_last_error.has_value()) {
      return *_last_error;
    } else if (_eof) {
      return std::nullopt;
    }
  }
  return output;
}

OrError<vector<string>> FileReader::read_all_lines()
{
  vector<string> output;
  while (true) {
    bail(line, read_line());
    if (!line.has_value()) { break; }
    output.push_back(*line);
  }
  return output;
}

template <class T> OrError<T> FileReader::_read_all_gen()
{
  T output;
  if (auto rem = remaining_bytes(); rem) { output.reserve(*rem); }
  while (_maybe_read_more()) {
    append_bytes(output, buffer_begin(), buffer_end());
    clear_buffer();
  }
  return std::move(output);
}

OrError<string> FileReader::read_all() { return _read_all_gen<string>(); }

OrError<vector<std::byte>> FileReader::read_all_bytes()
{
  return _read_all_gen<vector<std::byte>>();
}

OrError<char> FileReader::read_char()
{
  if (!_maybe_read_more()) {
    if (_last_error.has_value()) {
      return *_last_error;
    } else if (_maybe_read_more()) {
      return Error("EOF");
    } else {
      return Error("Unexpected error");
    }
  }
  return static_cast<char>(_buffer[_buffer_pos++]);
}

FileReader::FileReader(FD::shared_ptr&& fd) : _fd(std::move(fd)) {}
FileReader::FileReader(const FD::shared_ptr& fd) : _fd(fd) {}

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
    _last_error =
      EF("Failed to read requested bytes $: $", size, std::move(res.error()));
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
  auto ret = _read_into(_buffer, BufferSize);
  if (ret == 0) return false;
  _buffer_pos = 0;
  _buffer_size = ret;
  return true;
}

OrError<string> FileReader::read_file(const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all();
}

OrError<vector<string>> FileReader::read_file_lines(const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all_lines();
}

OrError<vector<std::byte>> FileReader::read_file_bytes(const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all_bytes();
}

bool FileReader::close() { return _fd->close(); }

const std::byte* FileReader::buffer_begin() const
{
  return _buffer + _buffer_pos;
}

const std::byte* FileReader::buffer_end() const
{
  return buffer_begin() + _available_on_buffer();
}

void FileReader::clear_buffer() { _buffer_pos = (_buffer_size = 0); }

OrError<size_t> FileReader::remaining_bytes()
{
  bail(from_fd, _fd->remaining_bytes());
  return from_fd + _available_on_buffer();
}

FileReader& FileReader::stdin_reader()
{
  static auto reader = from_fd(FD::stdin_filedesc());
  return *reader;
}

} // namespace bee
