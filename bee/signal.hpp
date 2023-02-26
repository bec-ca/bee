#pragma once

#include "error.hpp"
#include "file_descriptor.hpp"

namespace bee {

enum class SignalCode {
  SigChld,
  SigPipe,
};

struct Signal {
  static bee::OrError<bee::Unit> block_signal(SignalCode signal);

#ifdef __linux
  static bee::OrError<FileDescriptor> create_signal_fd(SignalCode signal);
#endif
};

} // namespace bee
