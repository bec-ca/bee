#pragma once

#include <cassert>
#include <deque>
#include <optional>
#include <string>
#include <variant>

#include "format.hpp"
#include "unit.hpp"

namespace bee {

struct ErrorLocation {
  std::string filename;
  int line;
};

struct ErrorMessage {
  std::string message;
  std::optional<ErrorLocation> location;
};

struct Error {
  explicit Error(const char* filename, int line, const std::string& msg);
  explicit Error(const char* filename, int line, std::string&& msg);
  explicit Error(const char* msg);
  explicit Error(const std::string& msg);
  explicit Error(std::string&& msg);

  Error(const Error& error);
  Error(Error&& error);

  Error& operator=(const Error& error);
  Error& operator=(Error&& error);

  template <class... Ts> static Error format(const char* fmt, Ts&&... args)
  {
    return Error(bee::format(fmt, std::forward<Ts>(args)...));
  }

  std::string msg() const;

  std::string to_string() const;

  std::string full_msg() const;

  const std::deque<ErrorMessage>& messages() const;

  void crash() const;
  void print_error() const;

  void add_tag(const std::string& tag);
  void add_tag(std::string&& tag);
  void add_tag(const char* tag);

  template <class... Ts> void add_tag(Ts&&... args)
  {
    if constexpr ((sizeof...(Ts)) >= 1) {
      add_tag(bee::format(std::forward<Ts>(args)...));
    }
  }

  void add_tag_with_location(
    const char* filename, int line, const std::string& tag);
  void add_tag_with_location(const char* filename, int line, std::string&& tag);
  void add_tag_with_location(const char* filename, int line, const char* tag);

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

template <class T> struct OrError {
 public:
  using value_type = T;

  OrError(const OrError& other) = default;
  OrError(OrError&& other) = default;

  OrError(const T& value) : _value(std::in_place_index<0>, value) {}
  OrError(T&& value) : _value(std::in_place_index<0>, std::move(value)) {}

  OrError(const Error& msg) : _value(std::in_place_index<1>, msg) {}
  OrError(Error&& msg) : _value(std::in_place_index<1>, std::move(msg)) {}

  template <class U>
    requires std::convertible_to<U, T>
  OrError(const OrError<U>& other)
      : _value(
          other.is_error() ? std::variant<T, Error>(other.error())
                           : std::variant<T, Error>(other.value()))
  {}

  template <class U>
    requires std::constructible_from<T, U>
  OrError(OrError<U>&& other)
      : _value(
          other.is_error()
            ? std::variant<T, Error>(std::move(other.error()))
            : std::variant<T, Error>(T(std::move(other.value()))))
  {}

  template <class U>
    requires std::constructible_from<T, U>
  OrError(U&& other) : _value(std::in_place_index<0>, std::forward<U>(other))
  {}

  OrError& operator=(const OrError& other) = default;
  OrError& operator=(OrError&& other) = default;

  OrError<Unit> ignore_value() const&
  {
    if (is_error()) {
      return error();
    } else {
      return unit;
    }
  }

  OrError<Unit> ignore_value() const&&
  {
    if (is_error()) {
      return std::move(error());
    } else {
      return unit;
    }
  }

  bool is_error() const { return std::holds_alternative<Error>(_value); }

  const Error& error() const { return std::get<Error>(_value); }
  Error& error() { return std::get<Error>(_value); }

  const T& value() const { return std::get<T>(_value); }
  T& value() { return std::get<T>(_value); }

  const T& operator*() const { return std::get<T>(_value); }
  T& operator*() { return std::get<T>(_value); }

  const T* operator->() const { return &std::get<T>(_value); }
  T* operator->() { return &std::get<T>(_value); }

  T value_default(T def)
  {
    if (is_error()) { return def; }
    return value();
  }

  T move_value_default(T def)
  {
    if (is_error()) { return def; }
    return std::move(value());
  }

  const T& force() const&
  {
    if (is_error()) { error().crash(); };
    return value();
  }

  T&& force() &&
  {
    if (is_error()) { error().crash(); };
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
      return format("Error($)", error());
    } else {
      return format("Ok($)", value());
    }
  }

 private:
  std::variant<T, Error> _value;
};

template <class T> OrError<T> join_error(const OrError<OrError<T>>& er)
{
  if (er.is_error()) {
    return er.error();
  } else {
    return er.value();
  }
}

inline OrError<Unit> ok() { return OrError<Unit>(unit); }

template <class T> OrError<T> ok(T&& value)
{
  return OrError<T>(std::forward<T>(value));
}

constexpr const char* maybe_format() { return ""; }

template <class... Ts> std::string maybe_format(Ts&&... args)
{
  return format(std::forward<Ts>(args)...);
}

#define shot(msg...)                                                           \
  do {                                                                         \
    return bee::Error(__FILE__, __LINE__, bee::maybe_format(msg));             \
  } while (false)

#define bail(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var)                                                               \
      .error()                                                                 \
      .add_tag_with_location(__FILE__, __LINE__, bee::maybe_format(msg));      \
    return std::move((__var##var).error());                                    \
  }                                                                            \
  auto& var = (__var##var).value();

#define bail_move(var, or_error, msg...)                                       \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var)                                                               \
      .error()                                                                 \
      .add_tag_with_location(__FILE__, __LINE__, bee::maybe_format(msg));      \
    return std::move((__var##var).error());                                    \
  }                                                                            \
  auto var = std::move((__var##var).value());

#define log_and_return_void(var, or_error, msg...)                             \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var)                                                               \
      .error()                                                                 \
      .add_tag_with_location(__FILE__, __LINE__, bee::maybe_format(msg));      \
    (__var##var).error().print_error();                                        \
    return;                                                                    \
  }                                                                            \
  auto& var = (__var##var).value();

#define bail_lhv(var, or_error, msg...)                                        \
  if ((or_error).is_error()) {                                                 \
    (or_error).error().add_tag_with_location(                                  \
      __FILE__, __LINE__, bee::maybe_format(msg));                             \
    return std::move((or_error).error());                                      \
  }                                                                            \
  auto& var = (or_error).value();

#define bail_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(                                     \
        __FILE__, __LINE__, bee::maybe_format(msg));                           \
      return __var.error();                                                    \
    }                                                                          \
    var = std::move(__var.value());                                            \
  } while (false)

#define must(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) {                                               \
    (__var##var)                                                               \
      .error()                                                                 \
      .add_tag_with_location(__FILE__, __LINE__, bee::maybe_format(msg));      \
    (__var##var).error().crash();                                              \
  }                                                                            \
  auto& var = (__var##var).value();

#define must_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      (__var).error().add_tag_with_location(                                   \
        __FILE__, __LINE__, bee::maybe_format(msg));                           \
      (__var).error().crash();                                                 \
    }                                                                          \
    var = std::move(__var.value());                                            \
  } while (false)

#define log_error(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(                                     \
        __FILE__, __LINE__, bee::maybe_format(msg));                           \
      __var.error().print_error();                                             \
    }                                                                          \
  } while (false)

#define bail_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(                                     \
        __FILE__, __LINE__, bee::maybe_format(msg));                           \
      return std::move((__var).error());                                       \
    }                                                                          \
  } while (false)

#define must_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) {                                                    \
      __var.error().add_tag_with_location(                                     \
        __FILE__, __LINE__, bee::maybe_format(msg));                           \
      __var.error().crash();                                                   \
    }                                                                          \
  } while (false)

} // namespace bee
