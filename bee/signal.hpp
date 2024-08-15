#pragma once

#include "fd.hpp"
#include "or_error.hpp"

namespace bee {

enum class SignalCode {
  SigChld,
  SigPipe,
};

struct Signal {
  static OrError<> block_signal(SignalCode signal);

#ifdef __linux
  static OrError<FD> create_signal_fd(SignalCode signal);
#endif
};

} // namespace bee
