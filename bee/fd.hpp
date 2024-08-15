#pragma once

#include <array>
#include <memory>

#include "data_buffer.hpp"
#include "file_mode.hpp"
#include "file_path.hpp"
#include "or_error.hpp"
#include "read_result.hpp"
#include "writer.hpp"

namespace bee {

struct FD final : public Writer {
  using shared_ptr = std::shared_ptr<FD>;
  using unique_ptr = std::unique_ptr<FD>;

  explicit FD(int fd);
  virtual ~FD() noexcept;

  FD(const FD& other) = delete;
  FD(FD&&) noexcept;

  FD& operator=(const FD& other) = delete;
  FD& operator=(FD&&) = delete;

  static OrError<FD> create_file(const FilePath& filename);
  static OrError<FD> open_file(const FilePath& filename);
  static OrError<FD> open_file(
    const FilePath& filename, const FileModeBitSet& mode);

  virtual bool close() override;
  bool is_closed();

  bool empty();

  OrError<> dup_onto(const FD& onto);
  OrError<FD> dup();

  shared_ptr to_shared() &&;
  unique_ptr to_unique() &&;

  OrError<size_t> send(const std::byte* data, size_t size);

  virtual OrError<size_t> write_raw(
    const std::byte* data, size_t size) override;

  OrError<ReadResult> read(std::byte* data, size_t size);
  OrError<ReadResult> recv(std::byte* data, size_t size);

  OrError<ReadResult> read(DataBuffer& buffer, size_t size);

  OrError<ReadResult> read_all_available(DataBuffer& buffer);
  OrError<ReadResult> recv_all_available(DataBuffer& buffer);

  OrError<std::optional<FD>> accept();

  OrError<> flush();

  int int_fd() const;

  bool is_tty() const;

  static const shared_ptr& stdout_filedesc();
  static const shared_ptr& stderr_filedesc();
  static const shared_ptr& stdin_filedesc();

  OrError<> set_blocking(bool blocking);

  bool is_write_blocked() const;

  OrError<> seek(size_t pos);
  OrError<> trunc(size_t size);
  OrError<size_t> remaining_bytes();

  OrError<bool> lock(bool shared = false, bool block = false);

 private:
  int _fd;
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
