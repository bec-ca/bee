#pragma once

#include <functional>

namespace bee {

struct OnDestroy {
  template <class T> OnDestroy(T&& fn) : _fn(std::forward<T>(fn)) {}
  ~OnDestroy() { _fn(); }

 private:
  std::function<void()> _fn;
};

} // namespace bee
