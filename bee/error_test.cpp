#include <stdexcept>

#include "error.hpp"
#include "format.hpp"
#include "testing.hpp"

using std::string;

namespace bee {
namespace {

TEST(map_lvalue)
{
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.map([](const string& str) { return str + str; });

    P(result);
  }
  {
    OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str + str; });

    P(result);
  }
}

TEST(map_rvalue)
{
  {
    auto result = OrError<string>(string("hello ")).map([](const string& str) {
      return str + str;
    });
    P(result);
  }
  {
    auto result = OrError<string>(Error("Error!")).map([](const string& str) {
      return str + str;
    });
    P(result);
  }
}

TEST(map_lvalue_diff_type)
{
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.map([](const string& str) { return str.size(); });

    P(result);
  }
  {
    OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str.size(); });

    P(result);
  }
}

TEST(map_rvalue_diff_type)
{
  {
    auto result = OrError<string>(string("hello ")).map([](const string& str) {
      return str.size();
    });
    P(result);
  }
  {
    auto result = OrError<string>(Error("Error!")).map([](const string& str) {
      return str.size();
    });
    P(result);
  }
}

TEST(bind_lvalue)
{
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.bind(
      [](const string& str) -> OrError<string> { return str + str; });

    P(result);
  }
  {
    OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind(
      [](const string& str) -> OrError<string> { return str + str; });

    P(result);
  }
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.bind([](const string& str) -> OrError<string> {
      return Error("Error2 " + str);
    });

    P(result);
  }
  {
    OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind([](const string& str) -> OrError<string> {
      return Error("Error2 " + str);
    });

    P(result);
  }
}

TEST(tag)
{
  auto run_test = []() -> OrError<Unit> {
    bail_unit(OrError<Unit>(Error("It didn't work!")), "trying something");
    return ok();
  };

  P(run_test());
}

TEST(compile_error)
{
  OrError<string> or_err(Error("Error1!"));

  auto result = or_err.bind([](const string&) { return ok(5); });

  P(result);
}

TEST(bail_unit)
{
  auto g = []() -> OrError<> { return Error("Test error"); };
  auto f = [&]() -> OrError<Unit> {
    bail_unit(g());
    return ok();
  };
  PRINT_EXPR(f());
}

TEST(try_with)
{
  PRINT_EXPR(try_with([&]() -> OrError<Unit> { return Error("foo"); }));
  PRINT_EXPR(
    try_with([&]() -> OrError<Unit> { throw std::runtime_error("boop"); }));
}

TEST(void_value)
{
  PRINT_EXPR([]() -> OrError<void> { return ok(); }());
  PRINT_EXPR([]() -> OrError<void> { return Error("failed"); }());
}

} // namespace
} // namespace bee
