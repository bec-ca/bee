#include "file_reader.hpp"

#include "file_writer.hpp"
#include "testing.hpp"

using std::string;

namespace bee {
namespace {

const char* tmp_filename = "/tmp/tmp2";

string create_content()
{
  string content;
  uint64_t c = 123498203984;
  for (int i = 0; i < 1 << 20; i++) {
    c = c * 13 + 7;
    content += char(c);
  }
  return content;
}

TEST(basic)
{
  string content = create_content();
  must_unit(FileWriter::save_file(tmp_filename, content));
  must(content_read, FileReader::read_file(tmp_filename));
  print_line(content_read == content);
  print_line(content.size());
}

TEST(read_by_blocks)
{
  string content = create_content();
  must_unit(FileWriter::save_file(tmp_filename, content));
  must(file, FileReader::open(tmp_filename));
  string content_read;
  char buf[1024];
  while (!file->is_eof()) {
    must(
      bytes_read, file->read(reinterpret_cast<std::byte*>(buf), sizeof(buf)));
    content_read.append(buf, bytes_read);
  }
  print_line(content_read == content);
  print_line(content.size());
}

} // namespace
} // namespace bee
