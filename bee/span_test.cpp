#include <numbers>

#include "format.hpp"
#include "span.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(to_string_of_string)
{
  auto r = [](Span span) {
    auto str = bee::to_string(span);
    auto s = Span::of_string(str);
    return F("$ $", str, s);
  };
  PRINT_EXPR(r(Span::zero()));
  PRINT_EXPR(r(Span::of_nanos(2)));
  PRINT_EXPR(r(Span::of_micros(2)));
  PRINT_EXPR(r(Span::of_millis(2)));
  PRINT_EXPR(r(Span::of_seconds(2)));
  PRINT_EXPR(r(Span::of_minutes(2)));
  PRINT_EXPR(r(Span::of_hours(2)));
  PRINT_EXPR(r(-Span::of_seconds(2)));

  PRINT_EXPR(r(Span::of_seconds(std::numbers::pi)));
}

TEST(of_string)
{
  PRINT_EXPR(Span::of_string("123"));
  PRINT_EXPR(Span::of_string("123s"));
  PRINT_EXPR(Span::of_string("1e9s"));
  PRINT_EXPR(Span::of_string("123xs"));
  PRINT_EXPR(Span::of_string("1.1.1s"));
  PRINT_EXPR(Span::of_string("-1.11s"));
  PRINT_EXPR(Span::of_string("--1.11s"));
}

TEST(to_string_with_format)
{
  P("{.2}", Span::of_seconds(1.2345));
  P("{.3}", Span::of_seconds(1.2345));
  P("{}", Span::of_seconds(1.2345));
}

} // namespace
} // namespace bee
