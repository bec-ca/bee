#pragma once

#include <cstdint>
#include <string>

namespace bee {

struct SimpleChecksum {
 private:
  using WordT = uint64_t;

 public:
  void add_string(const char* str, size_t size);
  void add_string(const std::byte* str, size_t size);
  void add_string(const std::string_view str);
  void maybe_flush();
  void force_flush();
  std::string hex();

  static std::string string_checksum(const std::string_view content);

 private:
  void _add_byte(std::byte c);
  void _add_word_unsafe(const WordT word);

  constexpr static size_t word_size = sizeof(WordT);

  std::byte _word[word_size];
  int _word_used = 0;
  WordT _acc = 18446744073709551557ull;
};

} // namespace bee
