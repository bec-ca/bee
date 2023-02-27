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

    print_line(result);
  }
  {
    OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str + str; });

    print_line(result);
  }
}

TEST(map_rvalue)
{
  {
    auto result = OrError<string>(string("hello ")).map([](const string& str) {
      return str + str;
    });
    print_line(result);
  }
  {
    auto result = OrError<string>(Error("Error!")).map([](const string& str) {
      return str + str;
    });
    print_line(result);
  }
}

TEST(map_lvalue_diff_type)
{
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.map([](const string& str) { return str.size(); });

    print_line(result);
  }
  {
    OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str.size(); });

    print_line(result);
  }
}

TEST(map_rvalue_diff_type)
{
  {
    auto result = OrError<string>(string("hello ")).map([](const string& str) {
      return str.size();
    });
    print_line(result);
  }
  {
    auto result = OrError<string>(Error("Error!")).map([](const string& str) {
      return str.size();
    });
    print_line(result);
  }
}

TEST(bind_lvalue)
{
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.bind(
      [](const string& str) -> OrError<string> { return str + str; });

    print_line(result);
  }
  {
    OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind(
      [](const string& str) -> OrError<string> { return str + str; });

    print_line(result);
  }
  {
    OrError<string> or_err(string("hello "));

    auto result = or_err.bind([](const string& str) -> OrError<string> {
      return Error("Error2 " + str);
    });

    print_line(result);
  }
  {
    OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind([](const string& str) -> OrError<string> {
      return Error("Error2 " + str);
    });

    print_line(result);
  }
}

TEST(tag)
{
  auto run_test = []() -> OrError<Unit> {
    bail_unit(OrError<Unit>(Error("It didn't work!")), "trying something");
    return ok();
  };

  print_line(run_test());
}

TEST(compile_error)
{
  OrError<string> or_err(Error("Error1!"));

  auto result = or_err.bind([](const string&) { return ok(5); });

  print_line(result);
}

} // namespace
} // namespace bee
