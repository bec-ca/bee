#pragma once

#include <cassert>
#include <deque>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include "exn.hpp"
#include "format.hpp"
#include "location.hpp"
#include "unit.hpp"

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
  Error(Error&& error);

  Error& operator=(const Error& error);
  Error& operator=(Error&& error);

  template <class... Ts> static Error fmt(const char* fmt, Ts&&... args)
  {
    return Error(F(fmt, std::forward<Ts>(args)...));
  }

  std::string msg() const;

  std::string to_string() const;

  std::string full_msg() const;

  const std::deque<ErrorMessage>& messages() const;

  void raise [[noreturn]] () const;
  void print_error() const;

  void add_tag(const std::string& tag);
  void add_tag(std::string&& tag);
  void add_tag(const char* tag);

  template <class... Ts> void add_tag(Ts&&... args)
  {
    if constexpr ((sizeof...(Ts)) >= 1) {
      add_tag(F(std::forward<Ts>(args)...));
    }
  }

  void add_tag_with_location(const Location& loc, const std::string& tag);
  void add_tag_with_location(const Location& loc, std::string&& tag);
  void add_tag_with_location(const Location& loc, const char* tag);

 private:
  std::deque<ErrorMessage> _messages;
};

template <class T> struct OrError;

template <class T> struct is_or_error_t;

template <class T> struct is_or_error_t<OrError<T>> {
  static constexpr bool value = true;
};

template <class T> struct is_or_error_t {
  static constexpr bool value = false;
};

namespace details {

template <class T>
concept or_error = is_or_error_t<T>::value;

template <class F, class T>
concept or_error_bind_fn = requires(const F& f, T&& t) {
                             {
                               f(std::forward<T>(t))
                               } -> or_error;
                           };

} // namespace details

struct ok_unit_t {};

template <class T = void> struct OrError {
 public:
  using value_type = T;
  using lvalue_type = std::add_lvalue_reference_t<T>;
  using rvalue_type = std::add_rvalue_reference_t<T>;
  using const_lvalue_type = std::add_lvalue_reference_t<const T>;
  using const_rvalue_type = std::add_rvalue_reference_t<const T>;

  OrError() {}

  OrError(const ok_unit_t&) {}

  template <std::convertible_to<T> U>
  OrError(const ok_unit_t&, U&& v)
      : _value(std::in_place_index<0>, std::forward<U>(v))
  {}

  OrError(const OrError& other) = default;
  OrError(OrError&& other) = default;

  template <std::convertible_to<T> U>
  OrError(U&& value) : _value(std::in_place_index<0>, std::forward<U>(value))
  {}

  OrError(const Error& msg) : _value(std::in_place_index<1>, msg) {}
  OrError(Error&& msg) : _value(std::in_place_index<1>, std::move(msg)) {}

  OrError(const bee::Exn& exn) : _value(Error(exn)) {}
  OrError(const std::exception& exn) : _value(Error(exn)) {}

  template <std::convertible_to<T> U>
  OrError(const OrError<U>& other)
      : _value(
          other.is_error() ? std::variant<T, Error>(other.error())
                           : std::variant<T, Error>(other.value()))
  {}

  template <std::convertible_to<T> U>
  OrError(OrError<U>&& other)
      : _value(
          other.is_error() ? std::variant<T, Error>(std::move(other.error()))
                           : std::variant<T, Error>(std::move(other.value())))
  {}

  OrError& operator=(const OrError& other) = default;
  OrError& operator=(OrError&& other) = default;

  template <std::convertible_to<T> U> OrError& operator=(U&& value)
  {
    _value = std::forward<U>(value);
    return *this;
  }

  template <class... Arg> lvalue_type emplace_value(Arg&&... arg)
  {
    return _value.emplace(std::forward<Arg>(arg)...);
  }

  template <std::convertible_to<T> U> OrError& operator=(OrError<U>&& value)
  {
    if (value.is_error()) {
      _value = std::move(value.error());
    } else {
      _value = std::move(value.value());
    }
    return *this;
  }

  OrError<> ignore_value() const&
  {
    if (is_error()) {
      return error();
    } else {
      return {};
    }
  }

  OrError<> ignore_value() &&
  {
    if (is_error()) {
      return std::move(error());
    } else {
      return {};
    }
  }

  bool is_error() const { return std::holds_alternative<Error>(_value); }

  const Error& error() const&
  {
    _raise_if_not_error();
    return std::get<Error>(_value);
  }

  Error& error() &
  {
    _raise_if_not_error();
    return std::get<Error>(_value);
  }

  const Error&& error() const&&
  {
    _raise_if_not_error();
    return std::move(std::get<Error>(_value));
  }

  Error&& error() &&
  {
    _raise_if_not_error();
    return std::move(std::get<Error>(_value));
  }

  const_lvalue_type value() const&
  {
    _raise_if_error();
    return std::get<T>(_value);
  }

  const_rvalue_type value() const&&
  {
    _raise_if_error();
    return std::move(std::get<T>(_value));
  }

  lvalue_type value() &
  {
    _raise_if_error();
    return std::get<T>(_value);
  }

  rvalue_type value() &&
  {
    _raise_if_error();
    return std::move(std::get<T>(_value));
  }

  const_lvalue_type operator*() const& { return value(); }
  lvalue_type operator*() & { return value(); }
  rvalue_type operator*() && { return value(); }
  const_rvalue_type operator*() const&& { return value(); }

  const T* operator->() const { return &value(); }
  T* operator->() { return &value(); }

  template <class U> T value_or(U&& def) const&
  {
    if (is_error()) { return std::forward<U>(def); }
    return value();
  }

  template <class U> T value_or(U&& def) &&
  {
    if (is_error()) { return std::forward<U>(def); }
    return std::move(value());
  }

  std::optional<T> to_optional() const&
  {
    if (is_error()) {
      return std::nullopt;
    } else {
      return value();
    }
  }

  std::optional<T> to_optional() &&
  {
    if (is_error()) {
      return std::nullopt;
    } else {
      return std::move(value());
    }
  }

  template <class F> auto bind(F&& f) const&
  {
    static_assert(
      std::invocable<F, T>,
      "bind function doesn't accept OrError inner value type");
    using P = typename std::invoke_result_t<F, T>;
    static_assert(
      details::or_error<P>,
      "bind function must return a value of type OrError");
    using U = typename P::value_type;
    if (!is_error()) {
      return OrError<U>(std::forward<F>(f)(value()));
    } else {
      return OrError<U>(error());
    }
  }

  template <class F> auto bind(F&& f) &&
  {
    static_assert(
      std::invocable<F, T>,
      "bind function doesn't accept OrError inner value type");
    using P = typename std::invoke_result_t<F, T>;
    static_assert(
      details::or_error<P>,
      "bind function must return a value of type OrError");
    using U = typename P::value_type;
    if (!is_error()) {
      return OrError<U>(std::forward<F>(f)(std::move(value())));
    } else {
      return OrError<U>(std::move(error()));
    }
  }

  template <class F>
    requires std::invocable<F, T>
  auto map(F&& f) const&
  {
    using U = typename std::invoke_result_t<F, T>;
    if (!is_error()) {
      return OrError<U>(std::forward<F>(f)(value()));
    } else {
      return OrError<U>(error());
    }
  }

  template <class F>
    requires std::invocable<F, T>
  auto map(F&& f) &&
  {
    using U = typename std::invoke_result_t<F, T>;
    if (!is_error()) {
      return OrError<U>(std::forward<F>(f)(std::move(value())));
    } else {
      return OrError<U>(std::move(error()));
    }
  }

  std::string to_string() const
  {
    if (is_error()) {
      return F("Error($)", error());
    } else {
      if constexpr (std::is_void_v<T>) {
        return F("Ok()");
      } else {
        return F("Ok($)", value());
      }
    }
  }

 private:
  void _raise_if_error() const
  {
    if (is_error()) {
      Error cpy = error();
      cpy.add_tag("OrError is in error");
      cpy.raise();
    }
  }

  void _raise_if_not_error() const
  {
    if (!is_error()) {
      throw bee::Exn("OrError::error called on instance that is not an error");
    }
  }

  std::variant<typename unit_if_void<T>::type, Error> _value;
};

template <class T> OrError<T> join_error(const OrError<OrError<T>>& er)
{
  if (er.is_error()) {
    return er.error();
  } else {
    return er.value();
  }
}

constexpr ok_unit_t ok() { return ok_unit_t{}; }

template <class T> constexpr OrError<T> ok(T&& value)
{
  return OrError<T>(ok(), std::forward<T>(value));
}

template <class F, class R = std::invoke_result_t<F>> OrError<R> try_with(F&& f)
{
  try {
    if constexpr (std::is_void_v<R>) {
      f();
      return ok();
    } else {
      return OrError<R>(ok(), f());
    }
  } catch (const bee::Exn& exn) {
    return OrError<R>(Error(exn));
  } catch (const std::exception& exn) {
    return OrError<R>(Error(exn));
  }
}

constexpr const char* maybe_format() { return ""; }

template <class... Ts> std::string maybe_format(Ts&&... args)
{
  return F(std::forward<Ts>(args)...);
}

#define shot(msg...)                                                           \
  do {                                                                         \
    return bee::Error(HERE, bee::maybe_format(msg));                           \
  } while (false)

#define raise_error(msg...) bee::Error(HERE, bee::maybe_format(msg)).raise();

#define bail(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var).error().add_tag_with_location(HERE, bee::maybe_format(msg));  \
    return std::move(__var##var).error();                                      \
  }                                                                            \
  auto& var = (__var##var).value();

#define log_and_return_void(var, or_error, msg...)                             \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var).error().add_tag_with_location(HERE, bee::maybe_format(msg));  \
    (__var##var).error().print_error();                                        \
    return;                                                                    \
  }                                                                            \
  auto& var = (__var##var).value();

#define bail_lhv(var, or_error, msg...)                                        \
  if ((or_error).is_error()) {                                                 \
    (or_error).error().add_tag_with_location(HERE, bee::maybe_format(msg));    \
    return std::move(or_error).error();                                        \
  }                                                                            \
  auto& var = (or_error).value();

#define bail_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(HERE, bee::maybe_format(msg));       \
      return __var.error();                                                    \
    }                                                                          \
    var = std::move(__var).value();                                            \
  } while (false)

#define must(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var).error().add_tag_with_location(HERE, bee::maybe_format(msg));  \
    (__var##var).error().raise();                                              \
  }                                                                            \
  auto& var = (__var##var).value();

#define must_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      (__var).error().add_tag_with_location(HERE, bee::maybe_format(msg));     \
      (__var).error().raise();                                                 \
    }                                                                          \
    var = std::move(__var).value();                                            \
  } while (false)

#define log_error(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(HERE, bee::maybe_format(msg));       \
      __var.error().print_error();                                             \
    }                                                                          \
  } while (false)

#define bail_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(HERE, bee::maybe_format(msg));       \
      return std::move(__var).error();                                         \
    }                                                                          \
  } while (false)

#define must_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(HERE, bee::maybe_format(msg));       \
      __var.error().raise();                                                   \
    }                                                                          \
  } while (false)

} // namespace bee
