#include "string_writer.hpp"

#include <memory>

using std::make_shared;
using std::string;

namespace bee {

StringWriter::~StringWriter() {}

StringWriter::ptr StringWriter::create() { return make_shared<StringWriter>(); }

bool StringWriter::close() { return true; }

OrError<size_t> StringWriter::write_raw(const std::byte* data, size_t size)
{
  auto c = reinterpret_cast<const char*>(data);
  _content.insert(_content.end(), c, c + size);
  return size;
}

string& StringWriter::content() { return _content; }

} // namespace bee
