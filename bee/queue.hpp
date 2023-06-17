#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <variant>

#include "span.hpp"

namespace bee {

template <class T> struct Queue {
 public:
  struct TimedOut {};
  struct QueueClosed {};

  struct GetResult {
   public:
    GetResult(const TimedOut& v) : _var(v) {}
    GetResult(const QueueClosed& v) : _var(v) {}
    GetResult(T&& v) : _var(std::move(v)) {}

    bool timed_out() const { return std::holds_alternative<TimedOut>(_var); }
    bool closed() const { return std::holds_alternative<QueueClosed>(_var); }

    T& operator*() { return std::get<T>(_var); }
    const T& operator*() const { return std::get<T>(_var); }

   private:
    std::variant<TimedOut, QueueClosed, T> _var;
  };

  struct EndIterator {};

  struct Iterator {
   public:
    Iterator(Queue<T>* queue) : _queue(queue), _value(_queue->pop()) {}

    T& operator*() { return *_value; }

    Iterator& operator++()
    {
      _value = _queue->pop();
      return *this;
    }

    bool operator==(const EndIterator&) const { return ended(); }

    bool ended() const { return !_value.has_value(); }

   private:
    Queue<T>* _queue;
    std::optional<T> _value;
  };

  Queue(std::optional<int> max_size = std::nullopt) : _max_size(max_size) {}

  Queue(const Queue& other) = delete;
  Queue(Queue&& other) = delete;

  Queue& operator=(Queue&& other) = delete;

  std::optional<T> pop_non_blocking()
  {
    auto lk = lock();
    return _pop_no_lock();
  }

  std::optional<T> pop()
  {
    auto lk = lock();
    _wait_readable(lk);
    return _pop_no_lock();
  }

  GetResult pop_with_timeout(Span timeout)
  {
    auto lk = lock();
    _wait_readable_with_timeout(lk, timeout);
    if (!_queue.empty()) {
      return std::move(*_pop_no_lock());
    } else if (_closed) {
      return QueueClosed{};
    } else {
      return TimedOut{};
    }
  }

  bool wait_available()
  {
    auto lk = lock();
    _wait_readable(lk);
    return !_queue.empty();
  }

  template <class U> bool push(U&& value)
  {
    auto lk = lock();
    _wait_writable(lk);
    if (_closed) { return false; }
    _queue.emplace_back(std::forward<U>(value));
    _read_cv.notify_one();
    return true;
  }

  void close()
  {
    auto lk = lock();
    if (_closed) return;
    _closed = true;
    _read_cv.notify_all();
    _write_cv.notify_all();
  }

  bool is_closed_and_empty() const
  {
    auto lk = lock();
    return _closed && _queue.empty();
  }

  Iterator begin() { return Iterator(this); }

  EndIterator end() { return EndIterator(); }

 private:
  using ul = std::unique_lock<std::mutex>;

  ul lock() const { return ul(_mutex); }

  std::optional<T> _pop_no_lock()
  {
    if (!_queue.empty()) {
      auto ret = std::move(_queue.front());
      _queue.pop_front();
      _write_cv.notify_one();
      return ret;
    } else {
      return std::nullopt;
    }
  }

  bool _is_readable() const { return !_queue.empty() || _closed; }

  void _wait_readable(ul& lk)
  {
    _read_cv.wait(lk, [this] { return _is_readable(); });
  }

  void _wait_readable_with_timeout(ul& lk, Span timeout)
  {
    _read_cv.wait_for(
      lk, timeout.to_chrono(), [this] { return _is_readable(); });
  }

  void _wait_writable(ul& lk)
  {
    if (!_max_size.has_value()) { return; }
    _write_cv.wait(
      lk, [this] { return std::ssize(_queue) < *_max_size || _closed; });
  }

  std::optional<int> _max_size;
  std::condition_variable _read_cv;
  std::condition_variable _write_cv;
  mutable std::mutex _mutex;
  std::list<T> _queue;
  bool _closed = false;
};

} // namespace bee
