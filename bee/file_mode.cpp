#include "file_mode.hpp"

#include <fcntl.h>

namespace bee {
namespace {

constexpr static std::array<FileMode, 5> all()
{
  return {
    FileMode::ReadOnly,
    FileMode::WriteOnly,
    FileMode::Create,
    FileMode::Truncate,
    FileMode::ReadWrite};
}

} // namespace

int FileModeBitSet::enum_to_system(FileMode v)
{
  switch (v) {
  case FileMode::ReadOnly:
    return O_RDONLY;
  case FileMode::WriteOnly:
    return O_WRONLY;
  case FileMode::Create:
    return O_CREAT;
  case FileMode::Truncate:
    return O_TRUNC;
  case FileMode::ReadWrite:
    return O_RDWR;
  }
}

int FileModeBitSet::to_system() const
{
  int mask = 0;
  for (const auto& mode : all()) {
    if (has(mode)) { mask |= enum_to_system(mode); }
  }
  return mask;
}

} // namespace bee
