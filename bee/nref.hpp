#pragma once

#include <cassert>
#include <memory>
#include <optional>

namespace bee {

// nref is somewhat like std::reference_wrapper, but offers -> and * operators,
// the underlying pointer can be null
template <class T> struct nref {
 public:
  constexpr nref() : _value(nullptr) {}
  constexpr nref(T& value) : _value(&value) {}
  constexpr nref(T* value) : _value(value) {}
  constexpr nref(const std::unique_ptr<T>& value) : _value(value.get()) {}
  constexpr nref(const std::shared_ptr<T>& value) : _value(value.get()) {}

  constexpr nref(std::nullptr_t) : _value(nullptr) {}

  constexpr nref(const nref& other) = default;

  constexpr nref& operator=(const nref& other) = default;

  constexpr nref operator=(T& value)
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

 private:
  T* _value;
};

} // namespace bee
