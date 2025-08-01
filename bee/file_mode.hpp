#pragma once

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

  constexpr FileModeBitSet(const FileMode m) : _value(static_cast<int>(m)) {}

  constexpr bool has(const FileMode m) const
  {
    return (_value & static_cast<int>(m)) != 0;
  }

  int to_system() const;

 private:
  constexpr FileModeBitSet(const int v) : _value(v) {}
  static int enum_to_system(FileMode v);

  int _value;
};

constexpr FileModeBitSet operator|(FileMode m1, FileMode m2)
{
  return FileModeBitSet(m1) | FileModeBitSet(m2);
}

} // namespace bee
