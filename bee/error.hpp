#pragma once

#include <deque>
#include <exception>
#include <optional>
#include <string>

#include "exn.hpp"
#include "format.hpp"
#include "location.hpp"

namespace bee {

struct ErrorMessage {
  std::string message;
  std::optional<Location> location;
};

struct Error {
  explicit Error(const Location& loc, const std::string& msg);
  explicit Error(const Location& loc, std::string&& msg);
  explicit Error(const char* msg);
  explicit Error(const std::string& msg);
  explicit Error(std::string&& msg);
  explicit Error(const bee::Exn& exn);
  explicit Error(const std::exception& exn);

  Error(const Error& error);
  Error(Error&& error) noexcept;

  Error& operator=(const Error& error);
  Error& operator=(Error&& error) noexcept;

  ~Error() noexcept;

  template <class... Ts> static Error fmt(const char* fmt, Ts&&... args)
  {
    return Error(F(fmt, std::forward<Ts>(args)...));
  }

  std::string msg() const;

  std::string to_string() const;

  std::string full_msg() const;

  const std::deque<ErrorMessage>& messages() const;

  void raise [[noreturn]] () const;

  void add_tag(const std::string& tag);
  void add_tag(std::string&& tag);
  void add_tag(const char* tag);

  void add_tag_with_location(const Location& loc, const std::string& tag);
  void add_tag_with_location(const Location& loc, std::string&& tag);
  void add_tag_with_location(const Location& loc, const char* tag);

 private:
  std::deque<ErrorMessage> _messages;
};

constexpr const char* maybe_format() { return ""; }

template <class... Ts> std::string maybe_format(Ts&&... args)
{
  return F(std::forward<Ts>(args)...);
}

#define EF(msg...) bee::Error(HERE, bee::maybe_format(msg))

#define shot(msg...)                                                           \
  do {                                                                         \
    return EF(msg);                                                            \
  } while (false)

#define raise_error(msg...) throw bee::Exn(HERE, bee::maybe_format(msg))

} // namespace bee
