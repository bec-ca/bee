#include <limits>

#include "exn.hpp"
#include "file_path.hpp"
#include "format.hpp"
#include "testing.hpp"

namespace bee {
namespace {

template <class T> void run_one_test()
{
  P("min:{}", std::numeric_limits<T>::min());
  P("max:{}", std::numeric_limits<T>::max());
  P("min:{,}", std::numeric_limits<T>::min());
  P("max:{,}", std::numeric_limits<T>::max());
};

template <class T> void run_test_impl()
{
  P("\nSigned:");
  run_one_test<T>();
  P("\nUnsigned:");
  run_one_test<std::make_unsigned_t<T>>();
};

TEST(basic)
{
  P("yo");
  P("num:{}", 5);
  P("time:{}{}", 5, "s");
  P("time:{{{}", 5);
  P("time:{{{{");
}

#define run_int_test(type)                                                     \
  P("--------------------------------");                                       \
  P("- $", #type);                                                             \
  P("--------------------------------");                                       \
  run_test_impl<type>();

TEST(int_format)
{
  P("num:{}", 1234567);
  P("num:{,}", 1234567);
  P("num:{,}", 234567);
  P("num:{,}", -234567);

  P("{+}", 1234567);
  P("{+}", -1234567);

  P("{+,}", 1234567);

  run_int_test(int8_t);
  run_int_test(int16_t);
  run_int_test(int32_t);
  run_int_test(int64_t);

  P(int32_t(1 << 31));
}

TEST(float_format_all_decimals)
{
  auto t = [](auto v) { return F("{.1000}", v); };
  PRINT_EXPR(t(float(1234567.0)));
  PRINT_EXPR(t(double(1234567.0)));
  PRINT_EXPR(t(float(-1234567.0)));
  PRINT_EXPR(t(double(-1234567.0)));

  PRINT_EXPR(t(0.0));
  PRINT_EXPR(t(std::numeric_limits<double>::max()));
  PRINT_EXPR(t(std::numeric_limits<double>::min()));
  PRINT_EXPR(t(std::numeric_limits<double>::infinity()));
  PRINT_EXPR(t(-std::numeric_limits<double>::infinity()));
  PRINT_EXPR(t(std::numeric_limits<double>::epsilon()));

  PRINT_EXPR(t(0.0 / 0.0));

  PRINT_EXPR(t(184467440737095516190000000000000000000.0));
  PRINT_EXPR(t(184467440737095516200000000000000000000.0));

  PRINT_EXPR(t(1e1));
  PRINT_EXPR(t(1e2));
  PRINT_EXPR(t(1e3));
  PRINT_EXPR(t(1e4));
  PRINT_EXPR(t(1e5));
  PRINT_EXPR(t(1e17));
  PRINT_EXPR(t(1e18));
  PRINT_EXPR(t(1e19));
  PRINT_EXPR(t(1e20));
  PRINT_EXPR(t(1e21));
  PRINT_EXPR(t(1e22));

  PRINT_EXPR(t(1e-17));
  PRINT_EXPR(t(1e-18));
  PRINT_EXPR(t(1e-19));
  PRINT_EXPR(t(1e-20));
  PRINT_EXPR(t(1e-21));
  PRINT_EXPR(t(1e-22));
}

TEST(float_format_comma)
{
  auto t = [](auto v) { return F("{,f}", v); };
  PRINT_EXPR(t(1.e17));
  PRINT_EXPR(t(1.e18));
  PRINT_EXPR(t(1.e19));
  PRINT_EXPR(t(1.e20));
  PRINT_EXPR(t(1.e21));
  PRINT_EXPR(t(1.e22));

  PRINT_EXPR(t(123412341.2341234123));
}

TEST(float_format_decimals)
{
  auto t = [](auto v) { return F("{,.2}", v); };

  PRINT_EXPR(t(0.9));
  PRINT_EXPR(t(0.99));
  PRINT_EXPR(t(0.999));
  PRINT_EXPR(t(0.995));
  PRINT_EXPR(t(0.994));
  PRINT_EXPR(t(123412341.2341234123));
}

TEST(float_format_sign)
{
  auto t = [](auto v) { return F("{,+}", v); };

  PRINT_EXPR(t(10));
  PRINT_EXPR(t(-10));
  PRINT_EXPR(t(1000));
  PRINT_EXPR(t(-1000));
  PRINT_EXPR(t(0.18));
  PRINT_EXPR(t(1000000));
}

template <class F> void print_exn(F&& fn)
{
  try {
    fn();
  } catch (const Exn& exn) {
    P("{}:{}",
      // Trim filename path to avoid test instability
      FilePath(exn.loc()->filename).filename(),
      exn.no_loc_what());
  }
}

TEST(raise_on_format_error)
{
  auto run_test = [](const char* fmt, const auto&... args) {
    P("=======");
    P("fmt: '$'", fmt);
    P("-------");
    print_exn([fmt, args...]() { P(fmt, args...); });
  };
  run_test("nothing");
  run_test("{}", 1);
  run_test("$", 1);
  run_test("{");
  run_test("{6}");
  run_test("other junk {} {c}", 1, 2);
  run_test("{}");
  run_test("$");
  run_test("$", 1, 2);
}

TEST(float_exponential_notation)
{
  auto t = [](auto v) { return F("{.8}", v); };
  PRINT_EXPR(t(float(1234567.0)));
  PRINT_EXPR(t(double(1234567.0)));
  PRINT_EXPR(t(float(-1234567.0)));
  PRINT_EXPR(t(double(-1234567.0)));

  PRINT_EXPR(t(0.0));
  PRINT_EXPR(t(std::numeric_limits<double>::max()));
  PRINT_EXPR(t(std::numeric_limits<double>::min()));
  PRINT_EXPR(t(std::numeric_limits<double>::infinity()));
  PRINT_EXPR(t(-std::numeric_limits<double>::infinity()));
  PRINT_EXPR(t(std::numeric_limits<double>::epsilon()));

  PRINT_EXPR(t(0.0 / 0.0));

  PRINT_EXPR(t(184467440737095516190000000000000000000.0));
  PRINT_EXPR(t(184467440737095516200000000000000000000.0));

  PRINT_EXPR(t(1e1));
  PRINT_EXPR(t(1e2));
  PRINT_EXPR(t(1e3));
  PRINT_EXPR(t(1e4));
  PRINT_EXPR(t(1e5));
  PRINT_EXPR(t(1e6));
  PRINT_EXPR(t(1e7));
  PRINT_EXPR(t(1e8));
  PRINT_EXPR(t(1e9));
  PRINT_EXPR(t(1e10));
  PRINT_EXPR(t(1e11));
  PRINT_EXPR(t(1e12));
  PRINT_EXPR(t(1e13));
  PRINT_EXPR(t(1e14));
  PRINT_EXPR(t(1e15));
  PRINT_EXPR(t(1e16));
  PRINT_EXPR(t(1e17));
  PRINT_EXPR(t(1e18));
  PRINT_EXPR(t(1e19));
  PRINT_EXPR(t(1e20));
  PRINT_EXPR(t(1e21));
  PRINT_EXPR(t(1e22));

  PRINT_EXPR(t(1e-1));
  PRINT_EXPR(t(1e-2));
  PRINT_EXPR(t(1e-3));
  PRINT_EXPR(t(1e-4));
  PRINT_EXPR(t(1e-5));
  PRINT_EXPR(t(1e-6));
  PRINT_EXPR(t(1e-7));
  PRINT_EXPR(t(1e-8));
  PRINT_EXPR(t(1e-9));
  PRINT_EXPR(t(1e-10));
  PRINT_EXPR(t(1e-11));
  PRINT_EXPR(t(1e-17));
  PRINT_EXPR(t(1e-18));
  PRINT_EXPR(t(1e-19));
  PRINT_EXPR(t(1e-20));
  PRINT_EXPR(t(1e-21));
  PRINT_EXPR(t(1e-22));
}

TEST(float_exact_decimal_places)
{
  auto t = [](auto v) { return F("{pf.2}", v); };
  PRINT_EXPR(t(7.0));
  PRINT_EXPR(t(0.0));
  PRINT_EXPR(t(700000.0));
  PRINT_EXPR(t(7.5));
  PRINT_EXPR(t(7.55));
  PRINT_EXPR(t(7.553));
  PRINT_EXPR(t(0.0700000));
  PRINT_EXPR(t(0.00007));
}

TEST(float_exact_decimal_places_zero)
{
  auto t = [](auto v) { return F("{pf.0}", v); };
  PRINT_EXPR(t(7.0));
  PRINT_EXPR(t(700000.0));
  PRINT_EXPR(t(7.5));
  PRINT_EXPR(t(7.55));
  PRINT_EXPR(t(7.553));
  PRINT_EXPR(t(0.0700000));
  PRINT_EXPR(t(0.00007));
}

TEST(int_space_padding)
{
  PRINT_EXPR(F("{ 5}", 5));
  PRINT_EXPR(F("{ 5}", 52));
  PRINT_EXPR(F("{ 5}", 1234));
  PRINT_EXPR(F("{ 5}", 12345));
  PRINT_EXPR(F("{ 5}", 123456));
}

TEST(int_zero_padding)
{
  PRINT_EXPR(F("{05}", 5));
  PRINT_EXPR(F("{05}", 52));
  PRINT_EXPR(F("{05}", 1234));
  PRINT_EXPR(F("{05}", 12345));
  PRINT_EXPR(F("{05}", 123456));
}

TEST(hex)
{
  PRINT_EXPR(F("{x}", 0));
  PRINT_EXPR(F("{x}", 9));
  PRINT_EXPR(F("{x}", 10));
  PRINT_EXPR(F("{x}", 10000));
  PRINT_EXPR(F("{x}", 0xffffffff));
  PRINT_EXPR(F("{x}", 0xc00004));
  PRINT_EXPR(F("{x}", 0xc0000004));

  PRINT_EXPR(F("{08x}", 0xabcd));
}

} // namespace
} // namespace bee
