#include "pretty_print.hpp"

#include "format.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(format_double)
{
  auto run_test = [](double value, int decimals) {
    print_line(
      "value:$ decimals:$ output:$",
      value,
      decimals,
      PrettyPrint::format_double(value, decimals));
  };
  run_test(5.0, 1);
  run_test(10.0, 1);
  run_test(123.13415, 2);
  run_test(123.13515, 2);
  run_test(123.13499, 2);
  run_test(123.135, 2);
  run_test(0.01, 3);
  run_test(0.01, 2);
  run_test(0.01, 1);
  run_test(1.345789, 5);
  run_test(1.345781, 5);
  run_test(1e-100, 5);
  run_test(1e+100, 5);
  run_test(12.154, 0);
  run_test(12.654, 0);
  run_test(5.7e-100, 101);
  run_test(5.7e-100, 100);

  run_test(-5.0, 1);
  run_test(-5.1, 1);
  run_test(-5.15, 1);
  run_test(-5.19, 1);
  run_test(-5.24999, 1);
}

} // namespace
} // namespace bee
