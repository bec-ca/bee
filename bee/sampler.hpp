#pragma once

#include <memory>
#include <random>
#include <vector>

namespace bee {

template <class T> struct Sampler {
 public:
  Sampler(int num_samples, uint32_t seed)
      : _num_samples(num_samples), _rng(seed)
  {}

  template <class U> void maybe_add(U&& item)
  {
    _seem_elements++;
    if (int(_sample.size()) < _num_samples) {
      _sample.push_back(std::forward<U>(item));
    } else {
      int r = _rand_int(_seem_elements);
      if (r < _num_samples) { _sample.at(r) = std::forward<U>(item); }
    }
  }

  const std::vector<T>& sample() const { return _sample; }
  std::vector<T>& sample() { return _sample; }

 private:
  int _rand_int(int n)
  {
    std::uniform_int_distribution<int> dist(0, n - 1);
    return dist(_rng);
  }

  std::vector<T> _sample;
  const int _num_samples;
  int _seem_elements = 0;

  std::mt19937_64 _rng;
};

} // namespace bee
