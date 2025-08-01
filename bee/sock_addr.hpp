#pragma once

#include <cstdint>
#include <variant>

#include "concepts.hpp"
#include "file_path.hpp"
#include "or_error.hpp"
#include "to_string_t.hpp"

namespace bee {

enum class SockAddrFamily {
  IPV4,
  IPV6,
  Unix,
};

struct SockAddr {
  struct IPV4 {
    uint32_t address;
    int port;

    std::string to_string() const;
  };

  struct IPV6 {
    std::array<uint32_t, 4> address{};
    int port;

    std::string to_string() const;
  };

  struct Unix {
    std::optional<FilePath> path;

    std::string to_string() const;
  };

  template <is_one_of<IPV4, IPV6, Unix> T>
  SockAddr(T&& addr) : address(std::forward<T>(addr))
  {}

  template <class Self, class F> decltype(auto) visit(this Self&& self, F&& f)
  {
    return std::visit(std::forward<F>(f), std::forward<Self>(self).address);
  }

  OrError<int> port() const;
  SockAddrFamily family() const;

  void set_port(int port);

  std::string to_string() const;

  std::variant<IPV4, IPV6, Unix> address;
};

template <> struct to_string_t<SockAddrFamily> {
  static const char* convert(SockAddrFamily values);
};

} // namespace bee
