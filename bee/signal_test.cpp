#include "signal.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(signal_name)
{
  PRINT_EXPR(SignalCode::SigInt);
  PRINT_EXPR(SignalCode::SigTerm);
}

} // namespace
} // namespace bee
