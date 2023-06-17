#pragma once

#include <memory>

#include "bee/error.hpp"

namespace bee {

struct Reader {
 public:
  using ptr = std::unique_ptr<Reader>;

  virtual ~Reader();

  virtual void close() = 0;
  virtual OrError<size_t> remaining_bytes() = 0;
  virtual OrError<std::string> read_str(size_t size) = 0;
  virtual bool is_eof() = 0;
};

} // namespace bee
