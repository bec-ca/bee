#pragma once

#include "bee/error.hpp"

#include <memory>

namespace bee {

struct Writer {
 public:
  using ptr = std::shared_ptr<Writer>;
  virtual ~Writer();
  virtual void close() = 0;
  virtual OrError<Unit> write(const std::string& data) = 0;
};

} // namespace bee
