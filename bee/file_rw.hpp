#pragma once

#include "file_path.hpp"
#include "reader.hpp"
#include "writer.hpp"

namespace bee {

struct FileRW : public Reader, public Writer {
 public:
  using ptr = std::unique_ptr<FileRW>;
  static OrError<ptr> open(const FilePath& filename);

  // Reader interface
  virtual OrError<size_t> remaining_bytes() override = 0;

  // Closer interface
  virtual bool close() override = 0;

  OrError<> seek(size_t pos);

 protected:
  virtual OrError<size_t> read_raw(std::byte* buffer, size_t size) override = 0;
  virtual OrError<size_t> write_raw(
    const std::byte* data, size_t size) override = 0;
};

} // namespace bee
