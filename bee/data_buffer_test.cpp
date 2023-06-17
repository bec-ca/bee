#include "data_buffer.hpp"
#include "format.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(read_line)
{
  DataBuffer b;
  auto print_lines = [&b]() {
    P("-----------------------");
    while (true) {
      auto line = b.read_line();
      if (!line.has_value()) { break; }
      P(*line);
    }
  };
  b.write("first line\nsecond line\nthird line\ntrailing line");
  print_lines();
  b.write(" some more data\nmore trailing\nyo\n");
  print_lines();
}

TEST(test_1_byte)
{
  DataBuffer b;
  b.write("1");
  b.write("2");
  b.consume(1);
  b.consume(1);
}

} // namespace
} // namespace bee
