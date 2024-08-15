#pragma once

#include <array>

namespace bee {

enum class FileMode {
  ReadOnly = 1,
  WriteOnly = 2,
  Create = 4,
  Truncate = 8,
  ReadWrite = 16,
};

struct FileModeBitSet {
 public:
  constexpr FileModeBitSet operator|(const FileModeBitSet& other) const
  {
    return FileModeBitSet(_value | other._value);
  }

  constexpr FileModeBitSet(FileMode m) : _value(int(m)) {}

  constexpr bool has(FileMode m) const { return (_value & int(m)) != 0; }

  int to_system() const;

 private:
  constexpr FileModeBitSet(int v) : _value(v) {}

  int _value;

  static int enum_to_system(FileMode v);
};

constexpr FileModeBitSet operator|(FileMode m1, FileMode m2)
{
  return FileModeBitSet(m1) | FileModeBitSet(m2);
}

} // namespace bee
