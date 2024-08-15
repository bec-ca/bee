#include "error.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(basic)
{
  Error err("foo");
  P(err);
}

} // namespace
} // namespace bee
