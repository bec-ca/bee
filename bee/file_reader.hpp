#pragma once

#include <memory>
#include <vector>

#include "fd.hpp"
#include "file_path.hpp"
#include "or_error.hpp"
#include "reader.hpp"

namespace bee {

struct FileReader : public Reader {
 public:
  using ptr = std::unique_ptr<FileReader>;

  FileReader();

  static OrError<ptr> open(const FilePath& filename);

  static ptr from_fd(const FD::shared_ptr& fd);

  static OrError<std::string> read_file(const FilePath& filename);
  static OrError<std::vector<std::byte>> read_file_bytes(
    const FilePath& filename);

  static OrError<std::vector<std::string>> read_file_lines(
    const FilePath& filename);

  static FileReader& stdin_reader();

  virtual ~FileReader() noexcept;

  virtual OrError<std::optional<std::string>> read_line() = 0;
  virtual OrError<std::vector<std::string>> read_all_lines() = 0;

  virtual OrError<std::string> read_all() = 0;
  virtual OrError<std::vector<std::byte>> read_all_bytes() = 0;
  virtual OrError<char> read_char() = 0;
};

} // namespace bee
