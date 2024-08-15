#pragma once

#include "reader.hpp"

namespace bee {

struct StringReader final : public Reader {
 public:
  using ptr = std::unique_ptr<StringReader>;

  StringReader(std::string&& content);
  virtual ~StringReader();

  static ptr create(std::string&& content);

  virtual bool close() override;
  virtual OrError<size_t> read_raw(std::byte* buffer, size_t size) override;
  virtual OrError<size_t> remaining_bytes() override;

 private:
  size_t _remaining_bytes() const;

  const std::string _content;
  size_t _offset = 0;
};

} // namespace bee
