#include "socket.hpp"

#include <cstring>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "errno_msg.hpp"

#include "bee/sock_addr.hpp"

#define bail_syscall(var, syscall, msg...)                                     \
  auto var = (syscall);                                                        \
  if (var < 0) [[unlikely]] {                                                  \
    auto err =                                                                 \
      Error::fmt("Syscall failed. Call:'$' error:'$'", #syscall, errno_msg()); \
    err.add_tag_with_location(HERE, maybe_format(msg));                        \
    return err;                                                                \
  }

#define bail_syscall_unit(syscall, msg...)                                     \
  if ((syscall) < 0) [[unlikely]] {                                            \
    auto err =                                                                 \
      Error::fmt("Syscall failed. Call:'$' error:'$'", #syscall, errno_msg()); \
    err.add_tag_with_location(HERE, maybe_format(msg));                        \
    return err;                                                                \
  }

namespace bee {
namespace {

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 0
#endif

union AddrUnion {
  sockaddr sa;
  sockaddr_in ipv4;
  sockaddr_in6 ipv6;
  sockaddr_un unix;
};

using AddrVariant = std::variant<sockaddr_in, sockaddr_in6, sockaddr_un>;

int accept_wrapper(const int fd, AddrUnion& addr)
{
  socklen_t len = sizeof(addr);
#ifdef __APPLE__
  return ::accept(fd, &addr.sa, &len);
#else
  return ::accept4(fd, &addr.sa, &len, SOCK_CLOEXEC);
#endif
}

int connect_wrapper(const int fd, const AddrVariant& addr)
{
  return std::visit(
    [&](const auto& addr) {
      return ::connect(
        fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    },
    addr);
}

int bind_wrapper(const int fd, const AddrVariant& addr)
{
  return std::visit(
    [&](const auto& addr) {
      return ::bind(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    },
    addr);
}

template <class T>
OrError<> set_sockopt(FD& fd, int level, int option_name, T value)
{
  bail_syscall_unit(
    ::setsockopt(fd.int_fd(), level, option_name, &value, sizeof(value)));
  return ok();
}

OrError<> set_tcp_nodelay(FD& fd)
{
  return set_sockopt(fd, IPPROTO_TCP, TCP_NODELAY, 1);
}

OrError<> set_cloexec(FD& fd)
{
  bail_syscall_unit(::fcntl(fd.int_fd(), F_SETFD, FD_CLOEXEC) != 0);
  return ok();
}

OrError<> prep_socket(const SockAddrFamily family, FD& fd)
{
  bail_unit(set_cloexec(fd));
  if (family == SockAddrFamily::IPV4 || family == SockAddrFamily::IPV6) {
    bail_unit(set_tcp_nodelay(fd));
  }
  return ok();
}

OrError<SockAddr> addr_of_os_addr(const AddrUnion& addr)
{
  switch (addr.sa.sa_family) {
  case AF_INET:
    return SockAddr::IPV4{
      .address = addr.ipv4.sin_addr.s_addr, .port = ntohs(addr.ipv4.sin_port)};
  case AF_INET6: {
    SockAddr::IPV6 out{.port = ntohs(addr.ipv6.sin6_port)};
    static_assert(sizeof(out.address) == sizeof(addr.ipv6.sin6_addr));
    memcpy(&out.address, &addr.ipv6.sin6_addr, sizeof(out.address));
    return out;
  }
  case AF_UNIX: {
    std::string str_path{addr.unix.sun_path};
    std::optional<FilePath> path;
    if (!str_path.empty()) path = FilePath{std::move(str_path)};
    return SockAddr::Unix{.path = std::move(path)};
  }
  default: raise_error("Unsupported address family $", addr.sa.sa_family);
  }
}

OrError<AddrVariant> addr_to_os_addr(const SockAddr& addr)
{
  return addr.visit([]<class T>(const T& addr) -> OrError<AddrVariant> {
    if constexpr (std::is_same_v<T, SockAddr::IPV4>) {
      sockaddr_in ipv4{};
      ipv4.sin_family = AF_INET;
      ipv4.sin_addr.s_addr = addr.address;
      ipv4.sin_port = htons(addr.port);
      return ipv4;
    } else if constexpr (std::is_same_v<T, SockAddr::IPV6>) {
      sockaddr_in6 ipv6{};
      ipv6.sin6_family = AF_INET6;
      static_assert(sizeof(addr.address) == sizeof(ipv6.sin6_addr));
      memcpy(&ipv6.sin6_addr, &addr.address, sizeof(addr.address));
      ipv6.sin6_port = htons(addr.port);
      return ipv6;
    } else if constexpr (std::is_same_v<T, SockAddr::Unix>) {
      sockaddr_un unix{};
      unix.sun_family = AF_UNIX;
      if (addr.path.has_value()) {
        const auto& path_str = addr.path.value().to_string();
        if (path_str.size() + 1 > sizeof(unix.sun_path)) {
          return EF("Unix socket path is too long");
        }
        strncpy(unix.sun_path, path_str.c_str(), sizeof(unix.sun_path));
      }
      return unix;
    }
  });
}

int to_os_family(const SockAddrFamily family)
{
  switch (family) {
  case SockAddrFamily::IPV4: return AF_INET;
  case SockAddrFamily::IPV6: return AF_INET6;
  case SockAddrFamily::Unix: return AF_UNIX;
  }
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// Socket
//

OrError<FD> Socket::create(const SockAddrFamily family)
{
  const int fd_int =
    ::socket(to_os_family(family), SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd_int < 0) { return EF("Failed to create socket: $", strerror(errno)); }
  FD fd(fd_int);
  bail_unit(prep_socket(family, fd));
  return fd;
}

OrError<std::optional<std::pair<FD, SockAddr>>> Socket::accept(FD& fd)
{
  AddrUnion addr;
  const int client_fd = accept_wrapper(fd.int_fd(), addr);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::nullopt;
    } else {
      return EF("Failed to accept incoming connection: $", errno_msg());
    }
  }
  FD out(client_fd);
  bail(out_addr, addr_of_os_addr(addr));
  bail_unit(prep_socket(out_addr.family(), out));
  return std::pair{std::move(out), out_addr};
}

OrError<SockAddr> Socket::get_sock_addr(const FD& fd)
{
  AddrUnion addr;
  socklen_t len = sizeof(addr);
  memset(&addr, 0, len);
  bail_syscall_unit(::getsockname(fd.int_fd(), &addr.sa, &len));
  return addr_of_os_addr(addr);
}

OrError<FD> Socket::connect_to_addr(const SockAddr& addr)
{
  bail(fd, create(addr.family()));
  bail(os_addr, addr_to_os_addr(addr));
  if (connect_wrapper(fd.int_fd(), os_addr) != 0) {
    return EF("Failed to connect to {}: {}", addr, errno_msg());
  }
  return std::move(fd);
}

OrError<FD> Socket::connect_to_host(const std::string& hostname, const int port)
{
  bail(addrs, resolve_address(hostname));
  if (addrs.empty()) { return EF("Address resolution returned no addresses"); }
  auto& addr = addrs.front();
  addr.set_port(port);
  return connect_to_addr(addr);
}

OrError<FD> Socket::listen(const SockAddr& addr)
{
  bail(fd, create(addr.family()));
  bail_unit(set_sockopt(fd, SOL_SOCKET, SO_REUSEADDR, 1));
  bail(os_addr, addr_to_os_addr(addr));
  bail_syscall_unit(bind_wrapper(fd.int_fd(), os_addr));
  bail_syscall_unit(::listen(fd.int_fd(), 16));
  return std::move(fd);
}

OrError<std::vector<SockAddr>> Socket::resolve_address(
  const std::string& hostname)
{
  addrinfo hints, *servinfo;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
  hints.ai_protocol = 0;

  const int ret = getaddrinfo(hostname.data(), nullptr, &hints, &servinfo);
  if (ret != 0) { return EF("Failed to resolve host: $", gai_strerror(ret)); }

  std::vector<SockAddr> output;
  for (auto p = servinfo; p != nullptr; p = p->ai_next) {
    auto h = reinterpret_cast<const AddrUnion*>(servinfo->ai_addr);
    bail(addr, addr_of_os_addr(*h));
    output.push_back(addr);
  }
  freeaddrinfo(servinfo);
  return output;
}

} // namespace bee
