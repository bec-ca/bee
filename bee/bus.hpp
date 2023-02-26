#pragma once

#include <functional>
#include <optional>
#include <vector>

namespace bee {

template <class T> struct Bus {
 public:
  Bus() {}
  ~Bus() {}

  void add_listener(const std::function<void(const T&)>& callback)
  {
    _listeners.push_back(callback);
    if (_value.has_value()) { callback(*_value); }
  }

  void set_value(const T& value)
  {
    _value = value;
    for (const auto& callback : _listeners) { callback(*_value); }
  }

  void clear_listeners() { _listeners.clear(); }

 private:
  std::optional<T> _value;
  std::vector<std::function<void(const T&)>> _listeners;
};

} // namespace bee
