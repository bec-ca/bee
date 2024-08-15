#pragma once

#include <stddef.h>

namespace bee {

struct ReadResult {
 public:
  explicit ReadResult(size_t bytes_read);

  static ReadResult eof();
  static ReadResult empty();

  bool is_eof() const;
  size_t bytes_read() const;

 private:
  explicit ReadResult(size_t bytes_read, bool is_eof);
  size_t _bytes_read;
  bool _is_eof;
};

} // namespace bee
