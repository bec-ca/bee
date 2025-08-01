#include "sock_addr.hpp"

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// SockAddr
//

std::string SockAddr::IPV4::to_string() const
{
  return F(
    "{}.{}.{}.{}:{}",
    address & 0xff,
    (address >> 8) & 0xff,
    (address >> 16) & 0xff,
    (address >> 24) & 0xff,
    port);
}

std::string SockAddr::IPV6::to_string() const
{
  const auto format_one = [](uint32_t part) {
    return F("{04x}:{04x}", part & 0xffff, part >> 16);
  };
  return F(
    "{}:{}:{}:{}:{}",
    format_one(address[0]),
    format_one(address[1]),
    format_one(address[2]),
    format_one(address[3]),
    port);
}

std::string SockAddr::Unix::to_string() const { return F(path); }

bee::OrError<int> SockAddr::port() const
{
  return visit([]<class T>(const T& addr) -> bee::OrError<int> {
    if constexpr (bee::is_one_of<T, IPV4, IPV6>) {
      return addr.port;
    } else if constexpr (std::is_same_v<T, Unix>) {
      return EF("Unix family has no port associated with it");
    }
  });
}

void SockAddr::set_port(const int port)
{
  visit([port]<class T>(T& addr) {
    if constexpr (bee::is_one_of<T, IPV4, IPV6>) {
      addr.port = port;
    } else if constexpr (std::is_same_v<T, Unix>) {
      raise_error("Unix family has no port associated with it");
    }
  });
}

SockAddrFamily SockAddr::family() const
{
  return visit([]<class T>(const T&) {
    if constexpr (std::is_same_v<T, IPV4>) {
      return SockAddrFamily::IPV4;
    } else if constexpr (std::is_same_v<T, IPV6>) {
      return SockAddrFamily::IPV6;
    } else if constexpr (std::is_same_v<T, Unix>) {
      return SockAddrFamily::Unix;
    }
  });
}

const char* to_string_t<SockAddrFamily>::convert(const SockAddrFamily v)
{
  switch (v) {
  case SockAddrFamily::IPV4: return "IPV4";
  case SockAddrFamily::IPV6: return "IPV6";
  case SockAddrFamily::Unix: return "Unix";
  }
}

std::string SockAddr::to_string() const
{
  return visit([]<class T>(const T& addr) -> std::string {
    if constexpr (std::is_same_v<T, IPV4>) {
      return F("[ipv4 $]", addr);
    } else if constexpr (std::is_same_v<T, IPV6>) {
      return F("[ipv6 $]", addr);
    } else if constexpr (std::is_same_v<T, Unix>) {
      return F("[unix $]", addr);
    }
  });
}

} // namespace bee
