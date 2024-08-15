#include "simple_checksum.hpp"

#include "print.hpp"

using std::string;

namespace bee {

void SimpleChecksum::add_string(const char* str, size_t size)
{
  for (size_t i = 0; i < size; i++) { _add_char(str[i]); }
}

void SimpleChecksum::add_string(const string& str)
{
  add_string(str.data(), str.size());
}

void SimpleChecksum::_add_char(char c)
{
  _word[_word_used++] = c;
  maybe_flush();
}

void SimpleChecksum::maybe_flush()
{
  if (_word_used == 8) {
    uint64_t c = *((uint64_t*)&_word[0]);
    _acc = _acc * 13 + c;
    _word_used = 0;
  }
}

void SimpleChecksum::force_flush()
{
  while (_word_used != 0) { _add_char(0); }
}

string SimpleChecksum::hex()
{
  force_flush();
  return F("{08x}", _acc);
}

string SimpleChecksum::string_checksum(const string& content)
{
  SimpleChecksum h;
  h.add_string(content);
  return h.hex();
}

} // namespace bee
