#include "string_writer.hpp"

using std::make_shared;
using std::string;

namespace bee {

StringWriter::~StringWriter() {}

StringWriter::ptr StringWriter::create() { return make_shared<StringWriter>(); }

void StringWriter::close() {}

OrError<> StringWriter::write(const string& data)
{
  _content += data;
  return ok();
}

string& StringWriter::content() { return _content; }

} // namespace bee
