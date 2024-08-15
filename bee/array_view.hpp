#pragma once

#include "bee/error.hpp"

namespace bee {

template <class T> struct ArrayView {
 public:
  constexpr ArrayView(T* const begin, T* const end) : _begin(begin), _end(end)
  {
    if (begin > end) {
      raise_error("Invalid range for array view [begin>end]");
    }
  }

  template <class C>
  constexpr ArrayView(C&& v) : _begin(v.data()), _end(v.data() + v.size())
  {}

  constexpr T* begin() const { return _begin; }
  constexpr T* end() const { return _end; }

  constexpr size_t size() const { return _end - _begin; }

  constexpr bool empty() const { return _begin == _end; }

  constexpr T& front() const { return at(0); }

  constexpr T& at(const size_t idx) const
  {
    if (idx >= size()) {
      raise_error("at: out of bounds access, idx:$ size:$", idx, size());
    }
    return _begin[idx];
  }

  constexpr T& operator[](const size_t idx) const { return at(idx); }

  constexpr ArrayView slice(const size_t begin) const
  {
    return ArrayView(_begin + begin, _end);
  }
  constexpr ArrayView slice(const size_t begin, const size_t end) const
  {
    return ArrayView(_begin + begin, _begin + end);
  }

 private:
  T* _begin;
  T* _end;
};

} // namespace bee
