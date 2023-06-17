#pragma once

#include <cstdint>
#include <string>

namespace bee {

template <size_t S> struct fixed_rstring {
 public:
  std::string to_string() const
  {
    return std::string(_data + _head, _data + S);
  }

  inline void prepend(char c) { _data[--_head] = c; }

  template <size_t N> inline void prepend(const std::array<char, N>& a)
  {
    for (size_t i = 0; i < N; i++) { _data[_head - i - 1] = a[i]; }
    _head -= 3;
  }

 private:
  char _data[S];
  size_t _head = S;
};

} // namespace bee
