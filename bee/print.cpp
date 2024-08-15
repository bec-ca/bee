#include "print.hpp"

#include "file_writer.hpp"

namespace bee {
namespace {

void write_line_and_maybe_flush(
  FileWriter& w, const std::string line, bool force_flush)
{
  must_unit(w.write_line(line));
  if (w.is_tty() || force_flush) { std::ignore = w.flush(); }
}

} // namespace

namespace details {

void print_file_str(LogOutput out, const std::string& line)
{
  switch (out) {
  case LogOutput::StdErr:
    write_line_and_maybe_flush(FileWriter::stderr(), line, true);
    break;
  case LogOutput::StdOut:
    write_line_and_maybe_flush(FileWriter::stdout(), line, false);
    break;
  }
}

} // namespace details

void flush_stdout() { std::ignore = FileWriter::stdout().flush(); }
void flush_stderr() { std::ignore = FileWriter::stderr().flush(); }

} // namespace bee
