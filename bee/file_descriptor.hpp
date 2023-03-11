#pragma once

#include "data_buffer.hpp"
#include "error.hpp"
#include "file_path.hpp"

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

  static OrError<FileDescriptor> create_file(const FilePath& filename);
  static OrError<FileDescriptor> open_file(const FilePath& filename);

  bool close();
  bool is_closed();

  bool empty();

  OrError<Unit> dup_onto(const FileDescriptor& onto);
  OrError<FileDescriptor> dup();

  shared_ptr to_shared() &&;
  unique_ptr to_unique() &&;

  OrError<size_t> send(const std::byte* data, size_t size);

  OrError<size_t> write(const std::byte* data, size_t size);
  OrError<size_t> write(const std::string& data);

  OrError<ReadResult> read(std::byte* data, size_t size);
  OrError<ReadResult> recv(std::byte* data, size_t size);

  OrError<ReadResult> read(DataBuffer& buffer, size_t size);

  OrError<ReadResult> read_all_available(DataBuffer& buffer);
  OrError<ReadResult> recv_all_available(DataBuffer& buffer);

  OrError<std::optional<FileDescriptor>> accept();

  OrError<Unit> flush();

  int int_fd() const;

  bool is_eof() const;

  bool is_tty() const;

  static const shared_ptr& stdout_filedesc();
  static const shared_ptr& stderr_filedesc();
  static const shared_ptr& stdin_filedesc();

  OrError<Unit> set_blocking(bool blocking);

  bool is_write_blocked() const;

  OrError<Unit> seek(size_t pos);
  OrError<size_t> remaining_bytes();

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

  static OrError<Pipe> create();
};

} // namespace bee
