#pragma once

#include <memory>
#include <vector>

#include "bytes.hpp"
#include "or_error.hpp"

namespace bee {

struct Reader {
 public:
  using ptr = std::unique_ptr<Reader>;

  virtual ~Reader() noexcept;

  virtual bool close() = 0;
  virtual OrError<size_t> remaining_bytes() = 0;

  [[nodiscard]] OrError<std::string> read_str(size_t size);
  [[nodiscard]] OrError<size_t> read(std::byte* buffer, size_t size);
  [[nodiscard]] OrError<size_t> read(Bytes& buffer, size_t size);

 protected:
  virtual OrError<size_t> read_raw(std::byte* buffer, size_t size) = 0;
};

} // namespace bee
