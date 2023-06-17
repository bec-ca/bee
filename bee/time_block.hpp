#pragma once

#include <functional>

#include "span.hpp"
#include "time.hpp"

namespace bee {

struct TimeBlock {
 public:
  template <class F>
  TimeBlock(F&& fn) : _start(Time::monotonic()), _fn(std::forward<F>(fn))
  {}

  ~TimeBlock() { _fn(Time::monotonic() - _start); }

 private:
  Time _start;
  std::function<void(Span)> _fn;
};

} // namespace bee
