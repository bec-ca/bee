#pragma once

#include <cassert>
#include <thread>
#include <vector>

#include "bee/pop_queue.hpp"
#include "bee/queue.hpp"

namespace bee {

struct ParallelMap {
  template <class T, class R>
  struct State : public std::enable_shared_from_this<State<T, R>> {
    using ptr = std::shared_ptr<State>;

    struct EndIterator {};

    struct Iterator {
     public:
      Iterator(ptr state) : _state(state), _it(state->output_queue->begin()) {}

      Iterator(const Iterator& other) = delete;
      Iterator(Iterator&& other) = default;

      R& operator*() { return *_it; }

      Iterator& operator++()
      {
        ++_it;
        if (_it.ended()) { _state->join(); }
        return *this;
      }

      bool operator==(const EndIterator&) const { return _it.ended(); }

     private:
      ptr _state;
      typename bee::Queue<R>::Iterator _it;
    };

    State() {}

    ~State() { assert(_joined); }

    State(const State& other) = delete;
    State(State&& other) = delete;

    void join()
    {
      assert(!_joined);
      waiter.join();
      _joined = true;
    }

    Iterator begin() { return Iterator(this->shared_from_this()); }
    EndIterator end() { return EndIterator(); }

    bee::Queue<R>::ptr output_queue = bee::Queue<R>::create();
    std::thread waiter;

   private:
    bool _joined = false;
  };

  template <class T, class R> struct StateWrapper {
   public:
    using S = State<T, R>;

    StateWrapper(const typename S::ptr& state) : _state(state) {}

    auto begin() { return _state->begin(); }
    auto end() { return _state->end(); }

    void join() { _state->join(); }

   private:
    typename S::ptr _state;
  };

  template <
    class Inputs,
    class F,
    class T = typename std::decay_t<Inputs>::value_type,
    std::move_constructible R = std::invoke_result_t<F, T>>
  static StateWrapper<T, R> go(Inputs&& inputs, int num_workers, F&& map)
  {
    auto input_queue = bee::PopQueue<T>::create(std::forward<Inputs>(inputs));
    auto state = std::make_shared<State<T, R>>();

    std::vector<std::thread> workers;
    for (int i = 0; i < num_workers; i++) {
      workers.emplace_back(
        [input_queue, output_queue = state->output_queue, map] {
          for (auto&& input : *input_queue) {
            output_queue->push(map(std::move(input)));
          }
        });
    }

    state->waiter = std::thread([workers = std::move(workers),
                                 output_queue = state->output_queue]() mutable {
      for (auto&& worker : workers) { worker.join(); }
      output_queue->close();
    });

    return state;
  }
};

} // namespace bee
