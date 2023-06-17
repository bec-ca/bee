#include "signal.hpp"

#include <csignal>
#include <cstring>

#include "fd.hpp"

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

OrError<sigset_t> create_mask(SignalCode signal)
{
  int signal_i = to_signal_int(signal);
  sigset_t mask;
  if (sigemptyset(&mask) != 0) {
    return Error::fmt("Failed to initialize sigset_t: $", strerror(errno));
  }
  if (sigaddset(&mask, signal_i) != 0) {
    return Error::fmt("Failed to set signal to sigset_t: $", strerror(errno));
  }

  return mask;
}

OrError<> block_mask(const sigset_t& mask)
{
  if (sigprocmask(SIG_BLOCK, &mask, nullptr) != 0) {
    return Error::fmt("Failed to block signal: $", strerror(errno));
  }

  return ok();
}

} // namespace

OrError<> Signal::block_signal(SignalCode signal)
{
  bail(mask, create_mask(signal));

  return block_mask(mask);
}

}; // namespace bee

#ifdef __linux

#include <sys/signalfd.h>

namespace bee {

OrError<FD> Signal::create_signal_fd(SignalCode signal)
{
  bail(mask, create_mask(signal));
  bail_unit(block_mask(mask));

  int fd = ::signalfd(-1, &mask, SFD_CLOEXEC | SFD_NONBLOCK);
  if (fd == -1) {
    return Error::fmt("Failed to create signalfd: $", strerror(errno));
  }

  return FD(fd);
}

} // namespace bee

#endif
