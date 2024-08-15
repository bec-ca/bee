#include <stdexcept>

#include "or_error.hpp"
#include "testing.hpp"

using std::string;

namespace bee::tmp {
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

struct Dummy {
 public:
  Dummy() {}
  Dummy(const Dummy&) noexcept : copy_constructed(true) {}
  Dummy(Dummy&&) noexcept : move_constructed(true) {}

  const char* to_string() const
  {
    if (copy_constructed) {
      return "copied";
    } else if (move_constructed) {
      return "moved";
    } else {
      return "neither";
    }
  }

 private:
  bool copy_constructed = false;
  bool move_constructed = false;
  std::string _dum;
};

TEST(return_copy)
{
  auto f1 = []() -> Dummy { return Dummy{}; };
  P(f1());
  auto f2 = []() -> OrError<Dummy> { return Dummy{}; };
  P(f2());
  auto f3 = []() -> OrError<Dummy> {
    Dummy x;
    return x;
  };
  P(f3());
}

static_assert(!std::convertible_to<OrError<void>, int>);
static_assert(!std::convertible_to<OrError<void>, bool>);
static_assert(!std::convertible_to<OrError<int>, int>);
static_assert(!std::convertible_to<int, OrError<std::string>>);

static_assert(std::convertible_to<OrError<int>, OrError<bool>>);
static_assert(std::convertible_to<OrError<const char*>, OrError<std::string>>);
static_assert(std::convertible_to<const char*, OrError<std::string>>);
static_assert(std::convertible_to<int, OrError<int>>);

} // namespace
} // namespace bee::tmp
