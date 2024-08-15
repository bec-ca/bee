#include "socket.hpp"

#include <cstring>

#include <sys/socket.h>

#ifdef __APPLE__

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 0
#endif

#endif

namespace bee {

bee::OrError<FD> create_socket_fd(int family)
{
  int fd = socket(family, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd == -1) { return EF("Failed to create socket: $", strerror(errno)); }
  return FD(fd);
}

} // namespace bee
