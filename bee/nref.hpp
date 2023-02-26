#pragma once

#include <memory>
#include <optional>

namespace bee {

template <class T> struct nref {
 public:
  inline constexpr nref(T& value) noexcept : _value(&value) {}
  inline constexpr nref(const std::unique_ptr<T>& value) noexcept
      : _value(value.get())
  {}
  inline constexpr nref(const std::shared_ptr<T>& value) noexcept
      : _value(value.get())
  {}

  inline constexpr nref(std::nullptr_t) noexcept : _value(nullptr) {}

  inline constexpr nref(const nref& other) = default;

  inline constexpr nref& operator=(const nref& other) = default;

  inline constexpr nref operator=(T& value)
  {
    _value = &value;
    return *this;
  }

  inline constexpr T& operator*() const noexcept
  {
    assert(has_value());
    return *_value;
  }

  inline constexpr T* operator->() const noexcept
  {
    assert(has_value());
    return _value;
  }

  inline constexpr T* get() const noexcept { return _value; }

  inline constexpr auto operator<=>(const nref& other) const noexcept = default;

  inline constexpr bool operator==(std::nullptr_t) const noexcept
  {
    return _value == nullptr;
  }

  inline constexpr bool has_value() const noexcept { return _value != nullptr; }

  constexpr operator bool() const noexcept { return has_value(); }

 private:
  T* _value;
};

} // namespace bee
