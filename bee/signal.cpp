#include "signal.hpp"

#include "file_descriptor.hpp"

#include <csignal>
#include <cstring>

namespace bee {

namespace {

int to_signal_int(SignalCode code)
{
  switch (code) {
  case SignalCode::SigChld:
    return SIGCHLD;
  case SignalCode::SigPipe:
    return SIGPIPE;
  default:
    assert(false && "This should not happen");
  }
}

bee::OrError<sigset_t> create_mask(SignalCode signal)
{
  int signal_i = to_signal_int(signal);
  sigset_t mask;
  if (sigemptyset(&mask) != 0) {
    return bee::Error::format(
      "Failed to initialize sigset_t: $", strerror(errno));
  }
  if (sigaddset(&mask, signal_i) != 0) {
    return bee::Error::format(
      "Failed to set signal to sigset_t: $", strerror(errno));
  }

  return mask;
}

bee::OrError<bee::Unit> block_mask(const sigset_t& mask)
{
  if (sigprocmask(SIG_BLOCK, &mask, nullptr) != 0) {
    return bee::Error::format("Failed to block signal: $", strerror(errno));
  }

  return bee::ok();
}

} // namespace

bee::OrError<bee::Unit> Signal::block_signal(SignalCode signal)
{
  bail(mask, create_mask(signal));

  return block_mask(mask);
}

}; // namespace bee

#ifdef __linux

#include <sys/signalfd.h>

namespace bee {

bee::OrError<FileDescriptor> Signal::create_signal_fd(SignalCode signal)
{
  bail(mask, create_mask(signal));
  bail_unit(block_mask(mask));

  int fd = ::signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
  if (fd == -1) {
    return bee::Error::format("Failed to create signalfd: $", strerror(errno));
  }

  return FileDescriptor(fd);
}

} // namespace bee

#endif
