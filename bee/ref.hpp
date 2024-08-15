#pragma once

#include <memory>
#include <optional>

#include "nref.hpp"

namespace bee {

// similar to bee:nref, but the underlying pointer cannot be null
template <class T> struct ref {
 public:
  constexpr ref(T* value) : _value(value) { assert(value != nullptr); }

  constexpr ref(T& value) : ref(&value) {}
  constexpr ref(const std::unique_ptr<T>& value) : ref(value.get()) {}
  constexpr ref(const std::shared_ptr<T>& value) : ref(value.get()) {}
  constexpr ref(const nref<T>& value) : ref(value.get()) {}

  constexpr ref(const ref& other) = default;
  constexpr ref& operator=(const ref& other) = default;

  constexpr ref operator=(T& value)
  {
    _value = &value;
    return *this;
  }

  constexpr T& operator*() const { return *_value; }

  constexpr T* operator->() const { return _value; }

  constexpr auto operator<=>(const ref& other) const = default;

 private:
  T* _value;
};

} // namespace bee
