#include "simple_checksum.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(basic)
{
  std::string text(100000, 55);
  P(SimpleChecksum::string_checksum(text));
}

TEST(not_aligned)
{
  SimpleChecksum sc;
  std::string text(100000, 55);

  sc.add_string("o");
  sc.add_string(text);
  P(sc.hex());
}

TEST(tail) { P(SimpleChecksum::string_checksum("hello world")); }

} // namespace
} // namespace bee
