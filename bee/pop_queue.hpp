#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <variant>

#include "span.hpp"

namespace bee {

template <std::move_constructible T> struct PopQueue {
 public:
  using ptr = std::shared_ptr<PopQueue>;

  struct EndIterator {};

  struct Iterator {
   public:
    Iterator(PopQueue<T>* queue) : _queue(queue), _value(_queue->pop()) {}

    T& operator*() { return *_value; }

    Iterator& operator++()
    {
      _value = _queue->pop();
      return *this;
    }

    bool operator==(const EndIterator&) const { return ended(); }

    bool ended() const { return !_value.has_value(); }

   private:
    PopQueue<T>* _queue;
    std::optional<T> _value;
  };

  template <class U> static ptr create(U&& els)
  {
    return std::make_shared<PopQueue>(std::forward<U>(els));
  }

  template <class U>
    requires std::constructible_from<std::vector<T>, U>
  PopQueue(U&& els) : _queue(std::forward<U>(els))
  {}

  PopQueue(const PopQueue& other) = delete;
  PopQueue(PopQueue&& other) = delete;
  PopQueue& operator=(PopQueue&& other) = delete;

  std::optional<T> pop()
  {
    size_t idx = _head.fetch_add(1);
    if (idx >= _queue.size()) return std::nullopt;
    return std::move(_queue[idx]);
  }

  Iterator begin() { return Iterator(this); }
  EndIterator end() { return EndIterator(); }

 private:
  std::vector<T> _queue;
  std::atomic<size_t> _head = 0;
};

} // namespace bee
