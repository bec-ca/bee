#include "alarms.hpp"

#include <memory>
#include <queue>

#include "queue.hpp"
#include "time.hpp"

using std::function;
using std::make_shared;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::vector;

template <class T, class C = std::less<T>>
using priority_queue = std::priority_queue<T, vector<T>, C>;

namespace bee {

namespace {

using item = std::pair<Span, std::function<void()>>;

void runner(shared_ptr<Queue<item>> q)
{
  using v = pair<Time, function<void()>>;

  struct cmp {
    inline bool operator()(const v& p1, const v& p2) const
    {
      return p1.first > p2.first;
    }
  };

  priority_queue<v, cmp> pqueue;

  while (true) {
    optional<Span> timeout;
    if (!pqueue.empty()) { timeout = pqueue.top().first - Time::monotonic(); }

    optional<item> el;
    if (!timeout.has_value()) {
      el = q->pop();
      if (!el.has_value()) { break; }
    } else if (*timeout > Span::zero()) {
      auto r = q->pop_with_timeout(*timeout);
      if (r.closed()) {
        break;
      } else if (!r.timed_out()) {
        el = std::move(*r);
      }
    } else {
      pqueue.top().second();
      pqueue.pop();
    }

    if (el.has_value()) {
      auto trigger = Time::monotonic() + el->first;
      pqueue.emplace(trigger, std::move(el->second));
    }
  }
}

} // namespace

Alarms::Alarms()
    : _queue(make_shared<Queue<item>>()), _runner_thread(runner, _queue)
{}

Alarms::~Alarms()
{
  _queue->close();
  _runner_thread.join();
}

void Alarms::add_alarm(Span timeout, std::function<void()>&& fn)
{
  _queue->push(make_pair(timeout, std::move(fn)));
}

} // namespace bee
