#pragma once

#include "fd.hpp"
#include "or_error.hpp"
#include "sock_addr.hpp"

namespace bee {

struct Socket {
  static OrError<FD> create(SockAddrFamily family);
  static OrError<FD> connect_to_addr(const SockAddr& addr);
  static OrError<FD> connect_to_host(const std::string& hostname, int port);
  static OrError<FD> listen(const SockAddr& addr);
  static OrError<std::optional<std::pair<FD, SockAddr>>> accept(FD& fd);
  static OrError<SockAddr> get_sock_addr(const FD& fd);
  static OrError<std::vector<SockAddr>> resolve_address(
    const std::string& hostname);
};

} // namespace bee
