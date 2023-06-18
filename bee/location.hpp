#pragma once

namespace bee {

struct Location {
  const char* filename;
  int line;
};

} // namespace bee

#define HERE (bee::Location{__FILE__, __LINE__})
