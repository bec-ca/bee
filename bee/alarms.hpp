#pragma once

#include "queue.hpp"
#include "span.hpp"

#include <functional>
#include <thread>

namespace bee {

struct Alarms {
 public:
  Alarms();
  ~Alarms();

  void add_alarm(Span timeout, std::function<void()>&&);

 private:
  using item = std::pair<Span, std::function<void()>>;

  std::shared_ptr<Queue<item>> _queue;
  std::thread _runner_thread;
};

} // namespace bee
