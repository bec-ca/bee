#pragma once

#include <exception>
#include <optional>
#include <string>

#include "location.hpp"

namespace bee {

struct Exn : public std::exception {
 public:
  Exn(const std::string_view what);
  Exn(const Location& loc, const std::string_view what);

  Exn(const Exn& other) noexcept;
  Exn(Exn&& other) noexcept;

  virtual ~Exn() noexcept;

  virtual const char* what() const noexcept override;

  const std::string& no_loc_what() const noexcept;

  const std::optional<Location>& loc() const noexcept;

  const std::string& to_string() const;

 private:
  std::optional<Location> _loc;
  std::string _what;
  std::string _no_loc_what;
};

} // namespace bee
