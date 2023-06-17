#pragma once

#include <memory>

#include "bee/error.hpp"

namespace bee {

struct Writer {
 public:
  using ptr = std::shared_ptr<Writer>;
  virtual ~Writer();
  virtual void close() = 0;
  virtual OrError<> write(const std::string& data) = 0;
};

} // namespace bee
