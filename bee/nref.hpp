#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <string>

#include "bee/to_string.hpp"

namespace bee {

// nref is somewhat like std::reference_wrapper, but offers -> and * operators,
// the underlying pointer can be null
template <class T> struct nref {
 public:
  constexpr nref() : _value(nullptr) {}
  constexpr nref(std::nullptr_t) : _value(nullptr) {}

  template <class U>
    requires std::convertible_to<U*, T*>
  constexpr nref(U& value) : _value(&value)
  {}

  template <class U>
    requires std::convertible_to<U*, T*>
  constexpr nref(U* const value) : _value(value)
  {}

  template <class U>
    requires std::convertible_to<U*, T*>
  constexpr nref(const std::unique_ptr<U>& value) : _value(value.get())
  {}

  template <class U>
    requires std::convertible_to<U*, T*>
  constexpr nref(const std::shared_ptr<U>& value) : _value(value.get())
  {}

  constexpr nref(const nref& other) = default;
  constexpr nref(nref&& other) = default;

  constexpr nref& operator=(const nref& other) = default;

  template <class U>
    requires std::convertible_to<U*, T*>
  constexpr nref operator=(U& value)
  {
    _value = &value;
    return *this;
  }

  constexpr T* get() const
  {
    assert(has_value());
    return _value;
  }

  constexpr T& operator*() const { return *get(); }

  constexpr T* operator->() const { return get(); }

  constexpr auto operator<=>(const nref& other) const = default;

  constexpr bool operator==(std::nullptr_t) const { return _value == nullptr; }

  constexpr bool has_value() const { return _value != nullptr; }

  constexpr operator bool() const { return has_value(); }

  std::string to_string() const
  {
    if (!has_value()) {
      return "nullptr";
    } else {
      return bee::to_string(*_value);
    }
  }

 private:
  T* _value;
};

} // namespace bee
