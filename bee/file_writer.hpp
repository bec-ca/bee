#pragma once

#include "bytes_buffer.hpp"
#include "error.hpp"
#include "file_descriptor.hpp"
#include "writer.hpp"

#include <array>
#include <memory>
#include <string>

namespace bee {

struct FileWriter final : public Writer {
 public:
  using ptr = std::unique_ptr<FileWriter>;

  static bee::OrError<ptr> create(const std::string& filename);

  FileWriter(const FileWriter&) = delete;
  FileWriter(FileWriter&& other) = delete;
  FileWriter& operator=(const FileWriter&) = delete;
  FileWriter& operator=(FileWriter&&) = delete;

  virtual ~FileWriter();

  bee::OrError<bee::Unit> write(const std::byte* data, size_t size);
  virtual bee::OrError<bee::Unit> write(const std::string& data) override;
  bee::OrError<bee::Unit> write(const std::vector<std::byte>& data);
  bee::OrError<bee::Unit> write(const DataBuffer& data);

  virtual void close() override;

  static bee::OrError<bee::Unit> save_file(
    const std::string& filename, const std::string& content);
  static bee::OrError<bee::Unit> save_file(
    const std::string& filename, const std::vector<std::byte>& content);

  bee::OrError<bee::Unit> flush();

 private:
  FileWriter(FileDescriptor&& fd);

  FileDescriptor _fd;

  BytesBuffer _buffer;
};

} // namespace bee
