#pragma once

#include <functional>
#include <memory>
#include <string>

#include "to_string.hpp"
#include "to_string_t.hpp"

namespace bee {

template <class T> struct to_string_t<std::shared_ptr<T>> {
  static std::string convert(const std::shared_ptr<T>& value)
  {
    if (value) {
      return bee::to_string(*value);
    } else {
      return "nullptr";
    }
  }
};

template <class T> struct to_string_t<std::unique_ptr<T>> {
  static std::string convert(const std::unique_ptr<T>& value)
  {
    if (value) {
      return bee::to_string(*value);
    } else {
      return "nullptr";
    }
  }
};

template <class T> struct to_string_t<T*> {
  static std::string convert(const T* value)
  {
    if (value) {
      return bee::to_string(*value);
    } else {
      return "nullptr";
    }
  }
};

template <class T> struct to_string_t<std::reference_wrapper<T>> {
  static std::string convert(const std::reference_wrapper<T> value)
  {
    return bee::to_string(value.get());
  }
};

} // namespace bee
