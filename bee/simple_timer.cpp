#include "simple_timer.hpp"

#include "print.hpp"

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// Timer
//

SimpleTimer::Timer::Timer(const std::string& name) : _name(name) {}

void SimpleTimer::Timer::add_call(const bee::Span& duration)
{
  _call_count.fetch_add(1);
  while (true) {
    auto tt = _total_time.load();
    if (_total_time.compare_exchange_strong(tt, tt + duration)) { break; }
  }
}

bee::Span SimpleTimer::Timer::total_time() const { return _total_time; }
int64_t SimpleTimer::Timer::call_count() const { return _call_count; }
const std::string& SimpleTimer::Timer::name() const { return _name; }

void SimpleTimer::Timer::reset()
{
  _call_count = 0;
  _total_time = bee::Span::zero();
}

////////////////////////////////////////////////////////////////////////////////
// Scoped
//

SimpleTimer::Scoped::Scoped(const Timer::ptr& timer)
    : _timer(timer), _start(bee::Time::monotonic())
{}

SimpleTimer::Scoped::~Scoped()
{
  auto duration = bee::Time::monotonic() - _start;
  _timer->add_call(duration);
}

////////////////////////////////////////////////////////////////////////////////
// SimpleTimer
//

SimpleTimer::SimpleTimer() {}
SimpleTimer::~SimpleTimer() {}

SimpleTimer& SimpleTimer::singleton()
{
  static SimpleTimer tracer;
  return tracer;
}

SimpleTimer::TimerWrapper SimpleTimer::create_timer(const std::string& name)
{
  auto timer = std::make_shared<Timer>(name);
  SimpleTimer::singleton()._register_timer(timer);
  return timer;
}

void SimpleTimer::_register_timer(const Timer::ptr& timer)
{
  _timers.push_back(timer);
}

void SimpleTimer::show_summary()
{
  for (const auto& timer : singleton()._timers) {
    auto count = timer->call_count();
    auto total_time = timer->total_time();
    if (count == 0) { continue; }
    auto avg_time = total_time / count;
    P("name:$ count:$ avg_time:$ total_time:$",
      timer->name(),
      count,
      avg_time,
      total_time);
    timer->reset();
  }
}

} // namespace bee
