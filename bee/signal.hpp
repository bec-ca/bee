#pragma once

#include "error.hpp"
#include "file_descriptor.hpp"

namespace bee {

enum class SignalCode {
  SigChld,
  SigPipe,
};

struct Signal {
  static OrError<Unit> block_signal(SignalCode signal);

#ifdef __linux
  static OrError<FileDescriptor> create_signal_fd(SignalCode signal);
#endif
};

} // namespace bee
