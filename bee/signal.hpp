#pragma once

#include "fd.hpp"
#include "or_error.hpp"

namespace bee {

enum class SignalCode {
  SigChld,
  SigPipe,
  SigInt,
  SigTerm,
};

template <> struct to_string_t<SignalCode> {
  static const char* convert(SignalCode);
};

struct Signal {
  static OrError<> block_signal(SignalCode signal);

  static OrError<> send(int pid, SignalCode signal);

  static void handle_signal(SignalCode, void (*signal_handler)(int));

  static SignalCode of_int(int signal);

#ifdef __linux
  static OrError<FD> create_signal_fd(SignalCode signal);
#endif
};

} // namespace bee
