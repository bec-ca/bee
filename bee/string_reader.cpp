#include "string_reader.hpp"

#include <cstring>

using std::string;

namespace bee {

StringReader::StringReader(string&& content) : _content(std::move(content)) {}

StringReader::~StringReader() {}

StringReader::ptr StringReader::create(string&& content)
{
  return make_unique<StringReader>(std::move(content));
}

bool StringReader::close() { return true; }

OrError<size_t> StringReader::read_raw(std::byte* buffer, size_t size)
{
  if (_offset >= _content.size()) { return 0; }
  size_t output_size = std::min(size, _remaining_bytes());
  memcpy(buffer, &_content[_offset], output_size);
  _offset += output_size;
  return output_size;
}

OrError<size_t> StringReader::remaining_bytes() { return _remaining_bytes(); }

size_t StringReader::_remaining_bytes() const
{
  return _content.size() - _offset;
}

} // namespace bee
