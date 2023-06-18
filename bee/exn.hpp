#pragma once

#include <concepts>
#include <exception>
#include <optional>
#include <string>

#include "location.hpp"

namespace bee {

struct Exn : public std::exception {
 public:
  template <std::convertible_to<std::string> T>
  Exn(T&& what) : _what(std::forward<T>(what))
  {}

  template <std::convertible_to<std::string> T>
  Exn(const Location& loc, T&& what) : _loc(loc), _what(std::forward<T>(what))
  {}

  Exn(const Exn& other) noexcept;

  virtual ~Exn();

  virtual const char* what() const noexcept override;

  const std::optional<Location>& loc() const noexcept;

 private:
  std::optional<Location> _loc;
  std::string _what;
};

} // namespace bee
