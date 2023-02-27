#pragma once

#include "writer.hpp"

namespace bee {

struct StringWriter final : public Writer {
 public:
  using ptr = std::shared_ptr<StringWriter>;

  virtual ~StringWriter();

  static ptr create();

  virtual void close() override;

  virtual OrError<Unit> write(const std::string& data) override;

  std::string& content();

 private:
  std::string _content;
};

} // namespace bee
