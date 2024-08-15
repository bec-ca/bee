#pragma once

#include "writer.hpp"

namespace bee {

struct StringWriter final : public Writer {
 public:
  using ptr = std::shared_ptr<StringWriter>;

  virtual ~StringWriter();

  static ptr create();

  virtual bool close() override;

  std::string& content();

 protected:
  virtual OrError<size_t> write_raw(
    const std::byte* data, size_t size) override;

 private:
  std::string _content;
};

} // namespace bee
