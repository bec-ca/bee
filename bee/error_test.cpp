#include "error.hpp"

#include "format.hpp"
#include "testing.hpp"

using std::string;

namespace bee {
namespace {

TEST(map_lvalue)
{
  {
    bee::OrError<string> or_err(string("hello "));

    auto result = or_err.map([](const string& str) { return str + str; });

    print_line(result);
  }
  {
    bee::OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str + str; });

    print_line(result);
  }
}

TEST(map_rvalue)
{
  {
    auto result =
      bee::OrError<string>(string("hello ")).map([](const string& str) {
        return str + str;
      });
    print_line(result);
  }
  {
    auto result =
      bee::OrError<string>(Error("Error!")).map([](const string& str) {
        return str + str;
      });
    print_line(result);
  }
}

TEST(map_lvalue_diff_type)
{
  {
    bee::OrError<string> or_err(string("hello "));

    auto result = or_err.map([](const string& str) { return str.size(); });

    print_line(result);
  }
  {
    bee::OrError<string> or_err(Error("Error!"));

    auto result = or_err.map([](const string& str) { return str.size(); });

    print_line(result);
  }
}

TEST(map_rvalue_diff_type)
{
  {
    auto result =
      bee::OrError<string>(string("hello ")).map([](const string& str) {
        return str.size();
      });
    print_line(result);
  }
  {
    auto result =
      bee::OrError<string>(Error("Error!")).map([](const string& str) {
        return str.size();
      });
    print_line(result);
  }
}

TEST(bind_lvalue)
{
  {
    bee::OrError<string> or_err(string("hello "));

    auto result = or_err.bind(
      [](const string& str) -> bee::OrError<string> { return str + str; });

    print_line(result);
  }
  {
    bee::OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind(
      [](const string& str) -> bee::OrError<string> { return str + str; });

    print_line(result);
  }
  {
    bee::OrError<string> or_err(string("hello "));

    auto result = or_err.bind([](const string& str) -> bee::OrError<string> {
      return bee::Error("Error2 " + str);
    });

    print_line(result);
  }
  {
    bee::OrError<string> or_err(Error("Error1!"));

    auto result = or_err.bind([](const string& str) -> bee::OrError<string> {
      return bee::Error("Error2 " + str);
    });

    print_line(result);
  }
}

TEST(tag)
{
  auto run_test = []() -> bee::OrError<bee::Unit> {
    bail_unit(
      bee::OrError<bee::Unit>(Error("It didn't work!")), "trying something");
    return bee::ok();
  };

  print_line(run_test());
}

TEST(compile_error)
{
  bee::OrError<string> or_err(Error("Error1!"));

  auto result = or_err.bind([](const string&) { return bee::ok(5); });

  print_line(result);
}

} // namespace
} // namespace bee
