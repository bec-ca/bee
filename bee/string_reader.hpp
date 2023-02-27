#pragma once

#include "reader.hpp"

namespace bee {

struct StringReader final : public Reader {
 public:
  using ptr = std::unique_ptr<StringReader>;

  StringReader(std::string&& content);
  virtual ~StringReader();

  static ptr create(std::string&& content);

  virtual void close() override;
  virtual OrError<std::string> read_str(size_t size) override;
  virtual OrError<size_t> remaining_bytes() override;
  virtual bool is_eof() override;

 private:
  const std::string _content;
  size_t _offset = 0;
};

} // namespace bee
