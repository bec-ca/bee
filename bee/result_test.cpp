#include "result.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(cto)
{
  Result<int, std::string> result;
  P(result);

  result = 5;
  P(result);

  result.emplace_error("error");
  P(result);

  result = std::string("error");
  P(result);
}

TEST(raise)
{
  Result<int, std::string> result;
  result = std::string("error");

  try {
    result.value();
  } catch (const bee::Exn& exn) {
    P("Exception raised:\n$", exn);
  }
}

TEST(raise_error)
{
  Result<int, Error> result;
  Error err("error");
  err.add_tag("tag");
  result = err;

  try {
    result.value();
  } catch (const bee::Exn& exn) {
    P("Exception raised:\n$", exn);
  }
}

TEST(bail)
{
  auto fn = []() -> Result<std::string, Error> {
    bail(v, (Result<std::string, Error>(Error("error"))), "Help!");
    return v;
  };
  P(fn());
}

TEST(bail_non_error)
{
  auto fn = []() -> Result<int, std::string> {
    bail(v, (Result<int, std::string>("error")));
    return v;
  };
  P(fn());
}

} // namespace
} // namespace bee
