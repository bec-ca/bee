#pragma once

#include <memory>

#include "data_buffer.hpp"
#include "error.hpp"
#include "file_path.hpp"

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

struct FD {
  using shared_ptr = std::shared_ptr<FD>;
  using unique_ptr = std::unique_ptr<FD>;

  explicit FD(int fd);
  ~FD();

  FD(const FD& other) = delete;
  FD(FD&&);

  FD& operator=(const FD& other) = delete;
  FD& operator=(FD&&) = delete;

  static OrError<FD> create_file(const FilePath& filename);
  static OrError<FD> open_file(const FilePath& filename);

  bool close();
  bool is_closed();

  bool empty();

  OrError<> dup_onto(const FD& onto);
  OrError<FD> dup();

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

  OrError<std::optional<FD>> accept();

  OrError<> flush();

  int int_fd() const;

  bool is_eof() const;

  bool is_tty() const;

  static const shared_ptr& stdout_filedesc();
  static const shared_ptr& stderr_filedesc();
  static const shared_ptr& stdin_filedesc();

  OrError<> set_blocking(bool blocking);

  bool is_write_blocked() const;

  OrError<> seek(size_t pos);
  OrError<size_t> remaining_bytes();

 private:
  int _fd;
  bool _eof;
  bool _write_blocked;
};

struct Pipe {
  ~Pipe();

  FD::shared_ptr read_fd;
  FD::shared_ptr write_fd;

  void close();

  static OrError<Pipe> create();
};

} // namespace bee
