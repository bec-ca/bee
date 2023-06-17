#pragma once

#include <array>
#include <memory>
#include <string>

#include "bytes_buffer.hpp"
#include "error.hpp"
#include "fd.hpp"
#include "file_path.hpp"
#include "writer.hpp"

namespace bee {

struct FileWriter final : public Writer {
 public:
  using ptr = std::unique_ptr<FileWriter>;

  static OrError<ptr> create(const FilePath& filename);

  FileWriter(const FileWriter&) = delete;
  FileWriter(FileWriter&& other) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter& operator=(FileWriter&&) = delete;

  virtual ~FileWriter();

  virtual OrError<> write(const std::string& data) override;
  OrError<> write(const std::byte* data, size_t size);
  OrError<> write(const std::vector<std::byte>& data);
  OrError<> write(const DataBuffer& data);
  // TODO: write function that takes DataBuffer&& and avoid copying to the
  // internal buffer

  virtual void close() override;

  static OrError<> save_file(
    const FilePath& filename, const std::string& content);
  static OrError<> save_file(
    const FilePath& filename, const std::vector<std::byte>& content);

  OrError<> flush();

 private:
  FileWriter(FD&& fd);

  FD _fd;

  BytesBuffer _buffer;
};

} // namespace bee
