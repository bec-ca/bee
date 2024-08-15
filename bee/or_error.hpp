#pragma once

#include <exception>
#include <type_traits>

#include "exn.hpp"
#include "result.hpp"

namespace bee {

template <class T = void> using OrError = Result<T, Error>;

template <class T>
OrError<typename T::value_type::value_type> join_error(T&& er)
{
  if (er.is_error()) {
    return std::move(er.error());
  } else {
    return std::move(er.value());
  }
}

template <class T> constexpr OrError<T> ok(T&& value)
{
  return OrError<T>(ok(), std::forward<T>(value));
}

template <class F, class R = std::invoke_result_t<F>> OrError<R> try_with(F&& f)
{
  try {
    if constexpr (std::is_void_v<R>) {
      std::forward<F>(f)();
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

} // namespace bee
