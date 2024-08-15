#pragma once

#include <memory>
#include <vector>

#include "fd.hpp"
#include "file_path.hpp"
#include "or_error.hpp"
#include "reader.hpp"

namespace bee {

struct FileReader final : public Reader {
 public:
  using ptr = std::unique_ptr<FileReader>;

  FileReader(FD::shared_ptr&& fd);
  FileReader(const FD::shared_ptr& fd);

  static OrError<ptr> open(const FilePath& filename);

  static ptr from_fd(const FD::shared_ptr& fd);

  static OrError<std::string> read_file(const FilePath& filename);
  static OrError<std::vector<std::byte>> read_file_bytes(
    const FilePath& filename);

  static OrError<std::vector<std::string>> read_file_lines(
    const FilePath& filename);

  FileReader(const FileReader&) = delete;
  FileReader(FileReader&& other) = delete;
  FileReader& operator=(const FileReader&) = delete;
  FileReader& operator=(FileReader&&) = delete;

  virtual ~FileReader() noexcept;

  OrError<std::optional<std::string>> read_line();
  OrError<std::vector<std::string>> read_all_lines();

  OrError<std::string> read_all();
  OrError<std::vector<std::byte>> read_all_bytes();
  OrError<char> read_char();

  virtual OrError<size_t> remaining_bytes() override;

  virtual bool close() override;

  static FileReader& stdin_reader();

 protected:
  [[nodiscard]] virtual OrError<size_t> read_raw(
    std::byte* buffer, size_t size) override;

 private:
  template <class T> OrError<T> _read_all_gen();

  size_t _available_on_buffer() const;
  bool _buffer_has_data() const;
  size_t _read_into(std::byte* buffer, size_t size);
  size_t _read_full(std::byte* buffer, size_t size);
  size_t _read_from_buffer(std::byte* buffer, size_t size);
  bool _maybe_read_more();

  const std::byte* buffer_begin() const;
  const std::byte* buffer_end() const;
  void clear_buffer();

  static constexpr size_t BufferSize = 1 << 13;

  FD::shared_ptr _fd;
  size_t _buffer_pos = 0;
  std::byte _buffer[BufferSize];
  size_t _buffer_size = 0;
  bool _eof = false;
  std::optional<Error> _last_error;
};

} // namespace bee
