#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace bee {

struct BytesBuffer {
 public:
  BytesBuffer() {}

  ~BytesBuffer() {}

  void write(const std::string& str) noexcept
  {
    for (char c : str) { _data.push_back(std::byte(c)); }
  }

  void write(std::byte c) noexcept { _data.push_back(c); }

  void write(const std::byte* data, size_t size) noexcept
  {
    for (size_t i = 0; i < size; i++) { _data.push_back(data[i]); }
  }

  const std::vector<std::byte>& data() const noexcept { return _data; }

  const std::byte* raw_data() const noexcept { return _data.data(); }

  size_t size() const noexcept { return _data.size(); }

  void reserve(size_t size) noexcept { _data.reserve(size); }

  void clear() { _data.clear(); }

  bool empty() const { return _data.empty(); }

 private:
  std::vector<std::byte> _data;
};

} // namespace bee
