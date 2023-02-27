#include "string_reader.hpp"

using std::string;

namespace bee {

StringReader::StringReader(string&& content) : _content(std::move(content)) {}

StringReader::~StringReader() {}

StringReader::ptr StringReader::create(string&& content)
{
  return make_unique<StringReader>(std::move(content));
}

void StringReader::close() {}

OrError<string> StringReader::read_str(size_t size)
{
  if (_offset >= _content.size()) { return ""; }
  size_t output_size = std::min(size, _content.size() - _offset);
  auto output = _content.substr(_offset, output_size);
  _offset += output_size;
  return output;
}

OrError<size_t> StringReader::remaining_bytes()
{
  return _content.size() - _offset;
}

bool StringReader::is_eof() { return _offset >= _content.size(); }

} // namespace bee
