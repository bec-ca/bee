#pragma once

#include <string>

namespace bee {

struct Location {
  const char* filename;
  int line;

  std::string to_string() const;
};

} // namespace bee

#define HERE (bee::Location{__FILE__, __LINE__})
