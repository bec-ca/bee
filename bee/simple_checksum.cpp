#include "simple_checksum.hpp"

#include "format.hpp"

namespace bee {

void SimpleChecksum::add_string(const std::byte* str, const size_t size)
{
  size_t idx = 0;

  while (_word_used != 0 && idx < size) _add_byte(str[idx++]);
  const auto num_words = (size - idx) / 8;
  for (size_t i = 0; i < num_words; i++) {
    _add_word_unsafe(*(reinterpret_cast<const WordT*>(&str[idx])));
    idx += word_size;
  }

  while (idx < size) _add_byte(str[idx++]);
}

void SimpleChecksum::add_string(const char* str, const size_t size)
{
  add_string(reinterpret_cast<const std::byte*>(str), size);
}

void SimpleChecksum::add_string(const std::string_view str)
{
  add_string(str.data(), str.size());
}

void SimpleChecksum::_add_byte(const std::byte c)
{
  _word[_word_used++] = c;
  maybe_flush();
}

void SimpleChecksum::maybe_flush()
{
  if (_word_used == 8) {
    _add_word_unsafe(*(reinterpret_cast<const WordT*>(_word)));
    _word_used = 0;
  }
}

void SimpleChecksum::_add_word_unsafe(const WordT w) { _acc = _acc * 13 + w; }

void SimpleChecksum::force_flush()
{
  while (_word_used != 0) { _add_byte(std::byte{0}); }
}

std::string SimpleChecksum::hex()
{
  force_flush();
  return F("{08x}", _acc);
}

std::string SimpleChecksum::string_checksum(const std::string_view content)
{
  SimpleChecksum h;
  h.add_string(content);
  return h.hex();
}

} // namespace bee
