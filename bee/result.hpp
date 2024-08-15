#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>
#include <variant>

#include "error.hpp"
#include "exn.hpp"
#include "format.hpp"
#include "format_params.hpp"
#include "to_string.hpp"
#include "unit.hpp"

namespace bee {

struct ok_unit_t {};

constexpr ok_unit_t ok() { return ok_unit_t{}; }

template <class T, class E> struct Result;

namespace details {

template <class T> struct is_result_t;

template <class T, class E> struct is_result_t<Result<T, E>> {
  static constexpr bool value = true;
};

template <class T> struct is_result_t {
  static constexpr bool value = false;
};

} // namespace details

template <class T>
concept is_result = details::is_result_t<T>::value;

template <class F, class T>
concept is_result_bind_fn = requires(const F& f, T&& t) {
  { f(std::forward<T>(t)) } -> is_result;
};

template <class T, class E> struct [[nodiscard]] Result {
 public:
  using value_type = T;
  using error_type = E;

  using variant_type = std::variant<typename unit_if_void<T>::type, E>;

  using lvalue_type = std::add_lvalue_reference_t<T>;

  Result() {}

  template <class... U>
  Result(const ok_unit_t&, U&&... v)
      : _value(std::in_place_index<0>, std::forward<U>(v)...)
  {}

  Result(const Result& other) = default;
  Result(Result&& other) noexcept = default;

  template <std::convertible_to<T> U>
    requires(!is_result<U>)
  Result(U&& value) : _value(std::in_place_index<0>, std::forward<U>(value))
  {}

  Result(const E& msg) : _value(std::in_place_index<1>, msg) {}
  Result(E&& msg) noexcept : _value(std::in_place_index<1>, std::move(msg)) {}

  template <std::convertible_to<T> U>
  Result(const Result<U, E>& other)
      : _value(
          other.is_ok()
            ? variant_type(std::in_place_index<0>, other.unchecked_value())
            : variant_type(std::in_place_index<1>, other.unchecked_error()))
  {}

  template <std::convertible_to<T> U>
  Result(Result<U, E>&& other)
      : _value(
          other.is_ok()
            ? variant_type(
                std::in_place_index<0>, std::move(other.unchecked_value()))
            : variant_type(
                std::in_place_index<1>, std::move(other.unchecked_error())))
  {}

  Result& operator=(const Result& other) = default;
  Result& operator=(Result&& other) noexcept = default;

  template <std::convertible_to<T> U>
    requires(!is_result<U>)
  Result& operator=(U&& value)
  {
    emplace_value(std::forward<U>(value));
    return *this;
  }

  template <class ET>
    requires(std::same_as<std::decay_t<ET>, E>)
  Result& operator=(ET&& value)
  {
    emplace_error(std::forward<ET>(value));
    return *this;
  }

  template <std::convertible_to<T> U> Result& operator=(Result<U, E>&& value)
  {
    if (value.is_ok()) {
      _value.emplace_error(std::move(value.unchecked_value()));
    } else {
      _value.emplace_value(std::move(value.unchecked_error()));
    }
    return *this;
  }

  template <class... Arg> lvalue_type emplace_value(Arg&&... arg)
  {
    return _value.template emplace<0>(std::forward<Arg>(arg)...);
  }

  template <class... Arg> E& emplace_error(Arg&&... arg)
  {
    return _value.template emplace<1>(std::forward<Arg>(arg)...);
  }

  template <class Self> Result<void, E> ignore_value(this Self&& self)
  {
    if (self.is_ok()) {
      return {};
    } else {
      return std::forward<Self>(self).unchecked_error();
    }
  }

  bool is_ok() const noexcept { return _value.index() == 0; }
  bool is_error() const noexcept { return _value.index() == 1; }

  explicit operator bool() const noexcept { return is_ok(); }

  template <class Self> auto&& unchecked_error(this Self&& self)
  {
    return std::get<1>(std::forward<Self>(self)._value);
  }

  template <class Self> auto&& error(this Self&& self)
  {
    self._raise_if_ok();
    return std::forward<Self>(self).unchecked_error();
  }

  template <class Self> auto&& unchecked_value(this Self&& self)
  {
    return std::get<0>(std::forward<Self>(self)._value);
  }

  template <class Self> auto&& value(this Self&& self)
  {
    self._raise_if_error();
    return std::forward<Self>(self).unchecked_value();
  }

  template <class Self> auto&& operator*(this Self&& self)
  {
    return std::forward<Self>(self).value();
  }

  template <class Self> auto* operator->(this Self&& self)
  {
    return &(std::forward<Self>(self).value());
  }
  template <class Self, class U> T value_or(this Self&& self, U&& def)
  {
    if (self.is_ok()) {
      return std::forward<Self>(self).value();
    } else {
      return std::forward<U>(def);
    }
  }

  template <class Self> std::optional<T> to_optional(this Self&& self)
  {
    if (self.is_ok()) {
      return std::forward<Self>(self).unchecked_value();
    } else {
      return std::nullopt;
    }
  }

  template <class Self, class F> auto bind(this Self&& self, F&& f)
  {
    static_assert(
      std::invocable<F, T>,
      "bind function doesn't accept Result inner value type");
    using P = typename std::invoke_result_t<F, T>;
    static_assert(
      is_result<P>, "bind function must return a value of type Result");
    using U = typename P::value_type;
    if (self.is_ok()) {
      return Result<U, E>(
        std::forward<F>(f)(std::forward<Self>(self).unchecked_value()));
    } else {
      return Result<U, E>(std::forward<Self>(self).unchecked_error());
    }
  }

  template <class Self, class F>
    requires std::invocable<F, T>
  auto map(this Self&& self, F&& f)
  {
    using U = typename std::invoke_result_t<F, T>;
    if (self.is_ok()) {
      return Result<U, E>(std::forward<F>(f)(std::forward<Self>(self).value()));
    } else {
      return Result<U, E>(std::forward<Self>(self).unchecked_error());
    }
  }

  std::string to_string(const FormatParams& p = {}) const
  {
    if (is_error()) {
      return F("Error($)", error());
    } else {
      if constexpr (std::is_void_v<T>) {
        return "Ok";
      } else {
        return bee::to_string(value(), p);
      }
    }
  }

 private:
  void _raise_if_error() const
  {
    if (is_error()) [[unlikely]] {
      auto err = [&]() -> Error {
        const auto& e = std::get<1>(_value);
        if constexpr (std::is_same_v<E, Error>) {
          return e;
        } else {
          return Error(bee::to_string(e));
        }
      }();
      err.add_tag("Result is in error");
      err.raise();
    }
  }

  void _raise_if_ok() const
  {
    if (is_ok()) { throw bee::Exn("Result is not an error"); }
  }

  std::variant<typename unit_if_void<T>::type, E> _value;
};

namespace details {

template <class T, class Args>
void maybe_add_location(Result<T, Error>& e, Location&& loc, Args&& args)
{
  std::apply(
    [&]<class... A>(A&&... a) {
      e.unchecked_error().add_tag_with_location(
        std::move(loc), bee::maybe_format(std::forward<A>(a)...));
    },
    std::forward<Args>(args));
}

template <class T, class E, class Args>
  requires(!std::same_as<E, Error>)
void maybe_add_location(Result<T, E>&, Location&&, Args&&)
{
  static_assert(
    std::tuple_size_v<Args> == 0,
    "Cannot add tag with when error type is not Error");
}

} // namespace details

#define bail(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) [[unlikely]] {                                  \
    bee::details::maybe_add_location(                                          \
      __var##var, HERE, std::forward_as_tuple(msg));                           \
    return std::move(__var##var).unchecked_error();                            \
  }                                                                            \
  auto& var = (__var##var).value()

#define bail_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) [[unlikely]] {                                       \
      bee::details::maybe_add_location(                                        \
        __var, HERE, std::forward_as_tuple(msg));                              \
      return __var.unchecked_error();                                          \
    }                                                                          \
    var = std::move(__var).value();                                            \
  } while (false)

#define bail_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) [[unlikely]] {                                       \
      bee::details::maybe_add_location(                                        \
        __var, HERE, std::forward_as_tuple(msg));                              \
      return std::move(__var).unchecked_error();                               \
    }                                                                          \
  } while (false)

#define must(var, or_error, msg...)                                            \
  auto __var##var = (or_error);                                                \
  if ((__var##var).is_error()) [[unlikely]] {                                  \
    bee::details::maybe_add_location(                                          \
      __var##var, HERE, std::forward_as_tuple(msg));                           \
    (__var##var).unchecked_error().raise();                                    \
  }                                                                            \
  auto& var = (__var##var).value()

#define must_assign(var, or_error, msg...)                                     \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) [[unlikely]] {                                       \
      bee::details::maybe_add_location(                                        \
        __var, HERE, std::forward_as_tuple(msg));                              \
      (__var).unchecked_error().raise();                                       \
    }                                                                          \
    var = std::move(__var).value();                                            \
  } while (false)

#define must_unit(or_error, msg...)                                            \
  do {                                                                         \
    auto __var = (or_error);                                                   \
    if (__var.is_error()) [[unlikely]] {                                       \
      bee::details::maybe_add_location(                                        \
        __var, HERE, std::forward_as_tuple(msg));                              \
      __var.unchecked_error().raise();                                         \
    }                                                                          \
  } while (false)

} // namespace bee
