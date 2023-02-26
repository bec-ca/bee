#pragma once

#include "data_buffer.hpp"
#include "error.hpp"

#include <memory>

namespace bee {

struct ReadResult {
 public:
  static ReadResult eof();

  bool is_eof() const;

  size_t bytes_read() const;

  static ReadResult create(size_t bytes_read, bool eof_reached);

  static ReadResult empty();

 private:
  ReadResult(size_t bytes_read, bool state);

  size_t _bytes_read;
  bool _is_eof;
};

struct FileDescriptor {
  using shared_ptr = std::shared_ptr<FileDescriptor>;
  using unique_ptr = std::unique_ptr<FileDescriptor>;

  explicit FileDescriptor(int fd);
  ~FileDescriptor();

  FileDescriptor(const FileDescriptor& other) = delete;
  FileDescriptor(FileDescriptor&&);

  FileDescriptor& operator=(const FileDescriptor& other) = delete;
  FileDescriptor& operator=(FileDescriptor&&) = delete;

  static bee::OrError<FileDescriptor> create_file(const std::string& filename);

  static bee::OrError<FileDescriptor> open_file(const std::string& filename);

  bool close();
  bool is_closed();

  bool empty();

  bee::OrError<bee::Unit> dup_onto(const FileDescriptor& onto);
  bee::OrError<FileDescriptor> dup();

  shared_ptr to_shared() &&;
  unique_ptr to_unique() &&;

  bee::OrError<size_t> send(const std::byte* data, size_t size);

  bee::OrError<size_t> write(const std::byte* data, size_t size);
  bee::OrError<size_t> write(const std::string& data);

  bee::OrError<ReadResult> read(std::byte* data, size_t size);
  bee::OrError<ReadResult> recv(std::byte* data, size_t size);

  bee::OrError<ReadResult> read(DataBuffer& buffer, size_t size);

  bee::OrError<ReadResult> read_all_available(DataBuffer& buffer);
  bee::OrError<ReadResult> recv_all_available(DataBuffer& buffer);

  bee::OrError<std::optional<FileDescriptor>> accept();

  bee::OrError<bee::Unit> flush();

  int int_fd() const;

  bool is_eof() const;

  bool is_tty() const;

  static const shared_ptr& stdout_filedesc();
  static const shared_ptr& stderr_filedesc();
  static const shared_ptr& stdin_filedesc();

  bee::OrError<bee::Unit> set_blocking(bool blocking);

  bool is_write_blocked() const;

  bee::OrError<bee::Unit> seek(size_t pos);
  bee::OrError<size_t> remaining_bytes();

 private:
  int _fd;
  bool _eof;
  bool _write_blocked;
};

struct Pipe {
  ~Pipe();

  FileDescriptor::shared_ptr read_fd;
  FileDescriptor::shared_ptr write_fd;

  void close();

  static bee::OrError<Pipe> create();
};

} // namespace bee
