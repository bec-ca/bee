#include "file_reader.hpp"

#include <cstring>

#include "fd.hpp"

namespace bee {
namespace {

////////////////////////////////////////////////////////////////////////////////
// FileReaderImpl
//

template <class T>
void append_bytes(T& output, const std::byte* begin, const std::byte* end)
{
  const size_t initial_size = output.size();
  const size_t num_bytes = end - begin;
  output.resize(initial_size + num_bytes);
  memcpy(&output[initial_size], begin, num_bytes);
}

struct FileReaderImpl final : public FileReader {
 public:
  using ptr = std::unique_ptr<FileReaderImpl>;

  FileReaderImpl(FD::shared_ptr&& fd) : _fd(std::move(fd)) {}
  FileReaderImpl(const FD::shared_ptr& fd) : _fd(fd) {}

  static OrError<ptr> open(const FilePath& filename)
  {
    bail(fd, FD::open_file(filename));
    return std::make_unique<FileReaderImpl>(std::move(fd).to_shared());
  }

  virtual ~FileReaderImpl() noexcept {}

  OrError<std::optional<std::string>> read_line() override
  {
    std::string output;
    bool found_eol = false;
    while (_maybe_read_more()) {
      const char c = static_cast<char>(_buffer[_buffer_begin_idx++]);
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

  OrError<std::vector<std::string>> read_all_lines() override
  {
    std::vector<std::string> output;
    while (true) {
      bail(line, read_line());
      if (!line.has_value()) { break; }
      output.push_back(*line);
    }
    return output;
  }

  OrError<std::string> read_all() override
  {
    return _read_all_gen<std::string>();
  }

  OrError<std::vector<std::byte>> read_all_bytes() override
  {
    return _read_all_gen<std::vector<std::byte>>();
  }

  OrError<char> read_char() override
  {
    if (!_maybe_read_more()) {
      if (_last_error.has_value()) {
        return *_last_error;
      } else if (_eof) {
        return Error("EOF");
      } else {
        return Error("Unexpected error");
      }
    }
    return static_cast<char>(_buffer[_buffer_begin_idx++]);
  }

  virtual OrError<size_t> remaining_bytes() override
  {
    bail(unread_bytes, _fd->remaining_bytes());
    return unread_bytes + _buffer_size();
  }

  virtual bool close() override { return _fd->close(); }

 protected:
  [[nodiscard]] virtual OrError<size_t> read_raw(
    std::byte* buffer, size_t size) override
  {
    size_t total_read = 0;

    const auto remaining = [&]() { return size - total_read; };

    total_read += _read_from_buffer(buffer, size);
    if (remaining() == 0) {
      // Nothing else to do there
    } else if (remaining() < BufferCapacity) {
      _maybe_read_more();
      total_read += _read_from_buffer(buffer + total_read, size - total_read);
    } else {
      total_read += _read_full(buffer + total_read, size - total_read);
    }

    if (total_read == 0 && _last_error.has_value()) { return *_last_error; }

    return total_read;
  }

 private:
  template <class T> OrError<T> _read_all_gen()
  {
    T output;
    if (auto rem = remaining_bytes(); rem) { output.reserve(*rem); }
    while (_maybe_read_more()) {
      append_bytes(output, _buffer_begin(), _buffer_end());
      _clear_buffer();
    }
    return std::move(output);
  }

  size_t _buffer_size() const { return _buffer_end_idx - _buffer_begin_idx; }

  bool _buffer_has_data() const { return _buffer_size() > 0; }

  size_t _read_into(std::byte* buffer, size_t size)
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

  size_t _read_full(std::byte* buffer, size_t size)
  {
    size_t bytes_read = 0;
    while (bytes_read < size) {
      auto ret = _read_into(buffer + bytes_read, size - bytes_read);
      if (ret == 0) break;
      bytes_read += ret;
    }
    return bytes_read;
  }

  size_t _read_from_buffer(std::byte* buffer, size_t size)
  {
    size_t to_copy = std::min(size, _buffer_size());
    memcpy(buffer, _buffer_begin(), to_copy);
    _buffer_begin_idx += to_copy;
    return to_copy;
  }

  bool _maybe_read_more()
  {
    if (_buffer_has_data()) return true;
    const size_t ret = _read_into(_buffer, BufferCapacity);
    if (ret == 0) return false;
    _buffer_begin_idx = 0;
    _buffer_end_idx = ret;
    return true;
  }

  const std::byte* _buffer_begin() const { return _buffer + _buffer_begin_idx; }

  const std::byte* _buffer_end() const { return _buffer + _buffer_end_idx; }

  void _clear_buffer()
  {
    _buffer_begin_idx = 0;
    _buffer_end_idx = 0;
  }

  static constexpr size_t BufferCapacity = 1 << 13;

  FD::shared_ptr _fd;
  std::byte _buffer[BufferCapacity];
  size_t _buffer_begin_idx = 0;
  size_t _buffer_end_idx = 0;
  bool _eof = false;
  std::optional<Error> _last_error;
};

} // namespace

////////////////////////////////////////////////////////////////////////////////
// FileReader
//

FileReader::FileReader() {}

OrError<FileReader::ptr> FileReader::open(const FilePath& filename)
{
  bail(fd, FD::open_file(filename));
  return std::make_unique<FileReaderImpl>(std::move(fd).to_shared());
}

FileReader::ptr FileReader::from_fd(const FD::shared_ptr& fd)
{
  return std::make_unique<FileReaderImpl>(fd);
}

FileReader::~FileReader() noexcept {}

OrError<std::vector<std::string>> FileReader::read_all_lines()
{
  std::vector<std::string> output;
  while (true) {
    bail(line, read_line());
    if (!line.has_value()) { break; }
    output.push_back(*line);
  }
  return output;
}

OrError<std::string> FileReader::read_file(const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all();
}

OrError<std::vector<std::string>> FileReader::read_file_lines(
  const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all_lines();
}

OrError<std::vector<std::byte>> FileReader::read_file_bytes(
  const FilePath& filename)
{
  bail(reader, FileReader::open(filename));
  return reader->read_all_bytes();
}

FileReader& FileReader::stdin_reader()
{
  static auto reader = from_fd(FD::stdin_filedesc());
  return *reader;
}

} // namespace bee
