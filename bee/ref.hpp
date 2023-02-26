#pragma once

#include <memory>
#include <optional>

#include "nref.hpp"

namespace bee {

template <class T> struct ref {
 public:
  inline constexpr ref(T* value) noexcept : _value(value)
  {
    assert(value != nullptr);
  }

  inline constexpr ref(T& value) noexcept : ref(&value) {}
  inline constexpr ref(const std::unique_ptr<T>& value) noexcept
      : ref(value.get())
  {}
  inline constexpr ref(const std::shared_ptr<T>& value) noexcept
      : ref(value.get())
  {}
  inline constexpr ref(const nref<T>& value) noexcept : ref(value.get()) {}

  inline constexpr ref(const ref& other) = default;

  inline constexpr ref& operator=(const ref& other) = default;

  inline constexpr ref operator=(T& value)
  {
    _value = &value;
    return *this;
  }

  inline constexpr T& operator*() const noexcept { return *_value; }

  inline constexpr T* operator->() const noexcept { return _value; }

  inline constexpr auto operator<=>(const ref& other) const noexcept = default;

  inline constexpr bool operator==(std::nullptr_t) const noexcept
  {
    return _value == nullptr;
  }

 private:
  T* _value;
};

} // namespace bee
