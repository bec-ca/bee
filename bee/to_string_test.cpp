#include "testing.hpp"
#include "to_string.hpp"

namespace bee {
namespace {

struct Dummy {
  Dummy() {}
  Dummy(const Dummy&) { P("copy ctor"); }
  Dummy(Dummy&&) { P("move ctor"); }
  const char* to_string() const& { return "&foo"; }
  const char* to_string() const&& { return "&&foo"; }
};

TEST(copy_rvalue) { P(to_string(Dummy())); }
TEST(copy_lvalue)
{
  Dummy d;
  P(to_string(d));
}

} // namespace
} // namespace bee
