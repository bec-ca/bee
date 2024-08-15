#pragma once

#include <array>
#include <memory>
#include <string>

#include "bytes_buffer.hpp"
#include "fd.hpp"
#include "file_path.hpp"
#include "or_error.hpp"
#include "writer.hpp"

namespace bee {

struct FileWriter final : public Writer {
 public:
  using ptr = std::unique_ptr<FileWriter>;

  FileWriter(FD&& fd, bool buffered);
  FileWriter(const FD::shared_ptr& fd, bool buffered);

  FileWriter(const FileWriter&) = delete;
  FileWriter(FileWriter&& other) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter& operator=(FileWriter&&) = delete;

  virtual ~FileWriter() noexcept;

  static OrError<ptr> create(const FilePath& filename);

  // TODO: write function that takes DataBuffer&& and avoid copying to the
  // internal buffer

  virtual bool close() override;

  static OrError<> write_file(
    const FilePath& filename, const std::string& content);
  static OrError<> write_file(
    const FilePath& filename, const std::vector<std::byte>& content);

  OrError<> flush();

  bool is_tty() const;

  static FileWriter& stdout();
  static FileWriter& stderr();

  void set_buffered(bool buffered);

 protected:
  virtual OrError<size_t> write_raw(
    const std::byte* data, size_t size) override;

 private:
  FD::shared_ptr _fd;

  BytesBuffer _buffer;

  const bool _is_tty;

  bool _buffered = true;
};

} // namespace bee
