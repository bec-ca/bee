#include "fd.hpp"

#include <cstring>
#include <filesystem>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

using std::string;

namespace bee {
namespace {

#define bail_syscall(var, syscall, msg...)                                     \
  auto var = (syscall);                                                        \
  if (var < 0) {                                                               \
    auto err = Error::fmt("$", strerror(errno));                               \
    err.add_tag_with_location(__FILE__, __LINE__, maybe_format(msg));          \
    return err;                                                                \
  }

#define bail_syscall_unit(syscall, msg...)                                     \
  if ((syscall) < 0) {                                                         \
    auto err = Error::fmt("$", strerror(errno));                               \
    err.add_tag_with_location(__FILE__, __LINE__, maybe_format(msg));          \
    return err;                                                                \
  }

char* errno_name() { return strerror(errno); }

OrError<std::pair<int, int>> agnostic_pipe()
{
  int fds[2];
#ifdef __APPLE__
  bail_syscall_unit(::pipe(fds));
  bail_syscall_unit(fcntl(fds[0], F_SETFD, FD_CLOEXEC));
  bail_syscall_unit(fcntl(fds[1], F_SETFD, FD_CLOEXEC));
#else
  bail_syscall(_, ::pipe2(fds, O_CLOEXEC));
#endif
  return std::make_pair(fds[0], fds[1]);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// ReadResult
//

ReadResult ReadResult::eof() { return ReadResult(0, true); }

ReadResult::ReadResult(size_t bytes_read, bool is_eof)
    : _bytes_read(bytes_read), _is_eof(is_eof)
{}

bool ReadResult::is_eof() const { return _is_eof; }

ReadResult ReadResult::create(size_t bytes_read, bool eof_reached)
{
  return ReadResult(bytes_read, eof_reached);
}

size_t ReadResult::bytes_read() const { return _bytes_read; }

ReadResult ReadResult::empty() { return ReadResult(0, false); }

////////////////////////////////////////////////////////////////////////////////
// FD
//

FD::FD(int fd) : _fd(fd), _eof(false), _write_blocked(false) {}

FD::FD(FD&& other)
    : _fd(other._fd), _eof(other._eof), _write_blocked(other._write_blocked)
{
  other._fd = -1;
}

FD::~FD() { close(); }

OrError<FD> FD::create_file(const FilePath& filename)
{
  bail_syscall(
    fd,
    ::open(filename.data(), O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC, 0644),
    "Failed to create file '$'",
    filename);
  return FD(fd);
}

OrError<FD> FD::open_file(const FilePath& filename)
{
  bail_syscall(
    fd,
    ::open(filename.data(), O_RDONLY | O_CLOEXEC),
    "Failed to open file '$'",
    filename);
  return FD(fd);
}

bool FD::close()
{
  if (_fd == -1) { return false; }
  auto ret = ::close(_fd);
  _fd = -1;
  return ret == 0;
}

bool FD::is_closed()
{
  if (_fd == -1) { return true; }
  return false;
}

OrError<ReadResult> FD::read(std::byte* data, size_t size)
{
  auto ret = ::read(_fd, data, size);
  if (ret == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      return ReadResult::empty();
    }
    return Error::fmt("Failed to read fd($): $", _fd, errno_name());
  }
  if (ret == 0) {
    _eof = true;
    return ReadResult::eof();
  } else {
    return ReadResult::create(ret, false);
  }
}

OrError<ReadResult> FD::read(DataBuffer& buffer, size_t size)
{
  std::byte bytes[1024];
  size_t bytes_read = 0;
  bool reached_eof = false;
  while (bytes_read < size) {
    bail(ret, read(bytes, std::min(size - bytes_read, sizeof(bytes))));
    buffer.write(bytes, ret.bytes_read());
    bytes_read += ret.bytes_read();
    if (ret.is_eof()) {
      reached_eof = true;
      break;
    }
  }
  return ReadResult::create(bytes_read, reached_eof);
}

OrError<ReadResult> FD::recv(std::byte* data, size_t size)
{
  auto ret = ::recv(_fd, data, size, 0);
  if (ret == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      return ReadResult::empty();
    } else if (errno == ECONNRESET) {
      return ReadResult::eof();
    }
    return Error::fmt("Failed to recv fd($): $", _fd, errno_name());
  }
  return ReadResult::create(ret, ret == 0);
}

OrError<ReadResult> FD::read_all_available(DataBuffer& output)
{
  std::byte buffer[1024 * 2];
  bool reached_eof = false;
  size_t bytes_read = 0;
  while (!reached_eof) {
    bail(ret, read(buffer, sizeof(buffer)));

    if (ret.is_eof()) { reached_eof = true; }

    if (ret.bytes_read() == 0) { break; }

    bytes_read += ret.bytes_read();
    output.write(buffer, ret.bytes_read());
  }
  return ReadResult::create(bytes_read, reached_eof);
}

OrError<ReadResult> FD::recv_all_available(DataBuffer& output)
{
  std::byte buffer[1024 * 2];
  bool reached_eof = false;
  size_t bytes_read = 0;
  while (!reached_eof) {
    bail(ret, recv(buffer, sizeof(buffer)));

    if (ret.is_eof()) { reached_eof = true; }

    if (ret.bytes_read() == 0) { break; }

    bytes_read += ret.bytes_read();
    output.write(buffer, ret.bytes_read());
  }
  return ReadResult::create(bytes_read, reached_eof);
}

OrError<size_t> FD::write(const std::byte* data, size_t size)
{
  _write_blocked = false;
  if (size == 0) { return 0; }
  auto ret = ::write(_fd, data, size);
  if (ret == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      _write_blocked = true;
      return 0;
    }
    return Error::fmt("Failed to write file: $", errno_name());
  }
  return ret;
}

OrError<size_t> FD::write(const string& data)
{
  return write(reinterpret_cast<const std::byte*>(data.data()), data.size());
}

OrError<> FD::dup_onto(const FD& onto)
{
  if (_fd == -1) return ok();
  bail_syscall(ret, dup2(_fd, onto._fd), "dup2 failed");
  return ok();
}

OrError<FD> FD::dup()
{
  if (_fd == -1) return Error("FD is not open");
  bail_syscall(fd, ::dup(_fd), "dup failed");
  return FD(fd);
}

std::shared_ptr<FD> FD::to_shared() &&
{
  return std::make_shared<FD>(std::move(*this));
}

std::unique_ptr<FD> FD::to_unique() &&
{
  return std::make_unique<FD>(std::move(*this));
}

OrError<> FD::flush()
{
  if (_fd != -1) {
    int ret = ::fsync(_fd);
    if (ret != 0) {
      if (errno != EROFS && errno != EINVAL) {
        return Error::fmt("fsync failed: $", errno_name());
      }
    }
  }
  return ok();
}

int FD::int_fd() const { return _fd; }

const FD::shared_ptr& FD::stdout_filedesc()
{
  static auto fd = FD(STDOUT_FILENO).to_shared();
  return fd;
}

const FD::shared_ptr& FD::stderr_filedesc()
{
  static auto fd = FD(STDERR_FILENO).to_shared();
  return fd;
}

const FD::shared_ptr& FD::stdin_filedesc()
{
  static auto fd = FD(STDIN_FILENO).to_shared();
  return fd;
}

bool FD::is_eof() const { return _eof; }

bool FD::is_tty() const { return ::isatty(_fd) == 1; }

OrError<> FD::set_blocking(bool blocking)
{
  bail_syscall(flags, fcntl(_fd, F_GETFL), "fcnt call failed");
  if (blocking) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }
  bail_syscall(ret, fcntl(_fd, F_SETFL, flags), "fcntl call failed");
  return ok();
}

bool FD::is_write_blocked() const { return _write_blocked; }

OrError<size_t> FD::send(const std::byte* data, size_t size)
{
  _write_blocked = false;
  if (size == 0) { return 0; }
  auto ret = ::send(_fd, data, size, 0);

  if (ret < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
      _write_blocked = true;
      return 0;
    } else {
      shot("Failed to send data: $", errno_name());
    }
  }

  return ret;
}

OrError<std::optional<FD>> FD::accept()
{
  int client_fd = ::accept(_fd, nullptr, nullptr);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::nullopt;
    } else {
      shot("Failed to accept incoming connection: $", strerror(errno));
    }
  }
  return FD(client_fd);
}

OrError<> FD::seek(size_t pos)
{
  if (_fd == -1) { return Error("file closed"); }
  bail_syscall_unit(lseek(_fd, pos, SEEK_SET));
  return ok();
}

OrError<size_t> FD::remaining_bytes()
{
  if (_fd == -1) { return Error("file closed"); }
  bail_syscall(cur, lseek(_fd, 0, SEEK_CUR));
  bail_syscall(size, lseek(_fd, 0, SEEK_END));
  bail_syscall_unit(lseek(_fd, cur, SEEK_SET));
  return size - cur;
}

////////////////////////////////////////////////////////////////////////////////
// Pipe
//

Pipe::~Pipe() {}

void Pipe::close()
{
  if (read_fd != nullptr) { read_fd->close(); }
  if (write_fd != nullptr) { write_fd->close(); }
}

OrError<Pipe> Pipe::create()
{
  bail(fds, agnostic_pipe());
  return Pipe{
    .read_fd = FD(fds.first).to_shared(),
    .write_fd = FD(fds.second).to_shared(),
  };
}

} // namespace bee
