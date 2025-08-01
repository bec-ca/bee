#include <functional>
#include <numbers>

#include "date.hpp"
#include "float_of_string.hpp"
#include "parse_string.hpp"
#include "print.hpp"
#include "simple_checksum.hpp"
#include "string_util.hpp"
#include "time.hpp"
#include "to_string.hpp"

namespace bee {
namespace {

struct BenchmarkInfo {
  void (*run)();
  std::string name;
};

std::vector<BenchmarkInfo>& benchs_singleton()
{
  static std::vector<BenchmarkInfo> benchs;
  return benchs;
}

int add_to_benchs(void (*run)(), const std::string& name)
{
  benchs_singleton().push_back({run, name});
  return 0;
}

#define BENCH(name)                                                            \
  void bench_##name();                                                         \
  const int _add_##name = add_to_benchs(bench_##name, #name);                  \
  void bench_##name()

template <class T> [[clang::noinline]] auto do_not_optimize_away(T&& t)
{
  auto* p = &t;
  asm volatile("" : "+r"(p));
}

template <class T> void time_it(const char* name, T&& f)
{
  const auto start = Time::now();
  int64_t repeat = 1;
  int64_t count = 0;
  auto output = f();
  auto threshold = Span::of_int_seconds(1);
  Span ellapsed = Span::zero();
  while (true) {
    for (int i = 0; i < repeat; i++) { do_not_optimize_away(f()); }
    count += repeat;
    ellapsed = (Time::now() - start);
    if (ellapsed > threshold) { break; }
    repeat *= 2;
  }
  P("-------------------------------");
  P("Name: $", name);
  P("output: $", output);
  P("{,}/call, calls:{}, total:{,}", ellapsed / count, count, ellapsed);
}

BENCH(SimpleChecksum)
{
  std::string text(100000, 55);
  time_it(
    "SimpleChecksum", [&]() { return SimpleChecksum::string_checksum(text); });
}

BENCH(format)
{
  time_it("Convert pi to string using std::to_string", []() {
    return std::to_string(std::numbers::pi);
  });
  time_it("Convert pi to string using bee::to_string", []() {
    return bee::to_string(std::numbers::pi);
  });

  time_it("Convert int32_t(pi*100000000) to str using bee::to_string", []() {
    return bee::to_string(int32_t(std::numbers::pi * 100000000));
  });
  time_it("Convert int32_t(pi*100000000) to str using std::to_string", []() {
    return std::to_string(int32_t(std::numbers::pi * 100000000));
  });

  time_it(
    "Convert int64_t(pi*1000000000000000000) to string using bee::to_string",
    []() {
      return bee::to_string(int64_t(std::numbers::pi * 1000000000000000000.0));
    });
  time_it(
    "Convert int64_t(pi*1000000000000000000) to string using std::to_string",
    []() {
      return std::to_string(int64_t(std::numbers::pi * 1000000000000000000.0));
    });
}

BENCH(date)
{
  time_it(
    "Convert date to string", []() { return (Date() + 3000000).to_string(); });
  time_it(
    "Convert string to date", []() { return Date::of_string("9999-01-01"); });

  time_it("Date from triple", []() { return Date(9999, 01, 01); });

  time_it("Date to triple", []() { return Date(9999, 01, 01).to_triple(); });
}

BENCH(time)
{
  time_it("Convert time to string", []() { return Time().to_string(); });
  time_it("Convert string to time", []() {
    return Time::of_string("2023-01-01 23:32:58.234");
  });
}

BENCH(parse)
{
  time_it(
    "bee::parse_string<int>", []() { return parse_string<int>("2012345678"); });

  time_it("std::stoi", []() { return std::stoi("2012345678"); });

  time_it("bee::parse_string<long long>", []() {
    return parse_string<long long>("2012345678123123123");
  });

  time_it("std::stoll", []() { return std::stoll("2012345678123123123"); });
}

BENCH(parse_float)
{
  std::string num = "1.2347902837e89";
  time_it("std::stod", [&]() { return std::stod(num); });
  time_it("bee::FloatOfStringExperimental<double>", [&]() {
    return bee::FloatOfString<double>::parse(num);
  });
}

BENCH(noop)
{
  time_it("noop", []() { return 5; });
}

const std::string sep(80, '=');
void print_banner(const std::string_view name)
{
  const auto title = right_pad_string(name, 80 - 6);
  P(sep);
  P("== $ ==", title);
  P(sep);
}

void main()
{
  for (const auto& bench : benchs_singleton()) {
    print_banner(bench.name);
    bench.run();
  }
  P(sep);
  P("Done!");
}

} // namespace
} // namespace bee

int main()
{
  bee::main();
  return 0;
}
