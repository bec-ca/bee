#pragma once

#include "reader.hpp"

#include "error.hpp"
#include "file_path.hpp"

#include <memory>
#include <vector>

namespace bee {

struct FileDescriptor;

struct FileReader final : public Reader {
 public:
  using ptr = std::unique_ptr<FileReader>;

  static OrError<ptr> open(const FilePath& filename);

  static OrError<std::string> read_file(const FilePath& filename);
  static OrError<std::vector<std::byte>> read_file_bytes(
    const FilePath& filename);

  FileReader(const FileReader&) = delete;
  FileReader(FileReader&& other) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader& operator=(FileReader&&) = delete;

  virtual ~FileReader();

  OrError<size_t> read(std::byte* buffer, size_t size);
  virtual OrError<std::string> read_str(size_t size) override;

  OrError<std::string> read_line();
  OrError<std::vector<std::string>> read_all_lines();

  OrError<std::string> read_all();
  OrError<std::vector<std::byte>> read_all_bytes();
  OrError<char> read_char();

  virtual OrError<size_t> remaining_bytes() override;

  virtual bool is_eof() override;

  virtual void close() override;

 private:
  FileReader(FileDescriptor&& fd);

  size_t _available_on_buffer() const;
  bool _buffer_has_data() const;
  size_t _read_into(std::byte* buffer, size_t size);
  size_t _read_full(std::byte* buffer, size_t size);
  size_t _read_from_buffer(std::byte* buffer, size_t size);
  bool _maybe_read_more();

  const std::byte* buffer_begin() const;
  const std::byte* buffer_end() const;
  void clear_buffer();

  std::unique_ptr<FileDescriptor> _fd;
  size_t _buffer_pos = 0;
  std::byte _buffer[1 << 13];
  size_t _buffer_size = 0;
  bool _eof = false;
  std::optional<Error> _last_error;
};

} // namespace bee
