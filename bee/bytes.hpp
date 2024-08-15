#pragma once

#include <vector>

#include "to_string_t.hpp"

namespace bee {

// TODO: Unify Bytes and BytesBuffer

struct Bytes : public std::vector<std::byte> {
  using std::vector<std::byte>::vector;

  explicit Bytes(const std::string& str);

  std::string to_string() const;
};

} // namespace bee
