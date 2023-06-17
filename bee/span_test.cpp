#include <numbers>

#include "format.hpp"
#include "span.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(to_string)
{
  P(F(Span::of_nanos(2)));
  P(F(Span::of_micros(2)));
  P(F(Span::of_millis(2)));
  P(F(Span::of_seconds(2)));
  P(F(Span::of_minutes(2)));
  P(F(Span::of_hours(2)));

  P(F(Span::of_seconds(std::numbers::pi)));
}

} // namespace
} // namespace bee
