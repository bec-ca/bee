#pragma once

#include <memory>
#include <vector>

#include "data_buffer.hpp"
#include "or_error.hpp"

namespace bee {

struct Writer {
 public:
  using ptr = std::shared_ptr<Writer>;
  virtual ~Writer() noexcept;
  virtual bool close() = 0;

  OrError<size_t> write(const std::byte* data, size_t size);
  OrError<size_t> write(const char* data, size_t size);
  OrError<size_t> write(const char* data);
  OrError<size_t> write(const std::vector<std::byte>& data);
  OrError<size_t> write(const DataBuffer& data);
  OrError<size_t> write(const std::string& data);

  OrError<size_t> write_line(const std::string& data);

 protected:
  virtual OrError<size_t> write_raw(const std::byte* data, size_t size) = 0;
};

} // namespace bee
