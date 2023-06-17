#pragma once

#include <cstdint>
#include <string>

namespace bee {

struct SimpleChecksum {
 public:
  void add_string(const char* str, size_t size);
  void add_string(const std::string& str);
  void maybe_flush();
  void force_flush();
  std::string hex();

  static std::string string_checksum(const std::string& content);

 private:
  void _add_char(char c);

  char _word[8];
  int _word_used = 0;
  uint64_t _acc = 18446744073709551557ull;
};

} // namespace bee
