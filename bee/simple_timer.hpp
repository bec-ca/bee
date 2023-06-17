#pragma once

#include <memory>
#include <vector>

#include "bee/span.hpp"
#include "bee/time.hpp"

namespace bee {

struct SimpleTimer {
 public:
  struct Scoped;

  struct Timer : public std::enable_shared_from_this<Timer> {
   public:
    using ptr = std::shared_ptr<Timer>;
    Timer(const std::string& name);

    Timer(const Timer& other) = delete;
    Timer(Timer&& other) = delete;

    Timer& operator=(const Timer& other) = delete;
    Timer& operator=(Timer&& other) = delete;

    void add_call(const bee::Span& duration);

    bee::Span total_time() const;
    int call_count() const;

    const std::string& name() const;

    void reset();

   private:
    std::string _name;

    bee::Span _total_time = bee::Span::zero();
    int64_t _call_count = 0;
  };

  struct Scoped {
   public:
    Scoped(const Timer::ptr& timer);
    ~Scoped();

   private:
    Timer::ptr _timer;
    const bee::Time _start;
  };

  struct TimerWrapper {
   public:
    TimerWrapper(const Timer::ptr& timer) : _timer(timer) {}

    Scoped operator()() const { return _timer; }

   private:
    Timer::ptr _timer;
  };

  ~SimpleTimer();

  SimpleTimer(const SimpleTimer& other) = delete;
  SimpleTimer(SimpleTimer&& other) = delete;

  SimpleTimer& operator=(const SimpleTimer& other) = delete;
  SimpleTimer& operator=(SimpleTimer&& other) = delete;

  static void show_summary();

  static SimpleTimer& singleton();

  static TimerWrapper create_timer(const std::string& name);

 private:
  SimpleTimer();

  void _register_timer(const std::shared_ptr<Timer>& timer);

  std::vector<std::shared_ptr<Timer>> _timers;
};

} // namespace bee
