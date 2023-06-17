#include <numbers>

#include "error.hpp"
#include "time.hpp"

#include "bee/to_string.hpp"

namespace bee {
namespace {

template <class T> void time_it(const char* name, T&& f)
{
  P("-------------------------------");
  P("Running $", name);
  auto start = Time::now();
  int count = 10000000;
  auto output = f();
  P("output: $", output);
  for (int i = 0; i < count; i++) { f(); }
  auto ellapsed = (Time::now() - start);
  P("{,}/call, total:{,}", ellapsed / count, ellapsed);
}

void run_benchmark()
{
  time_it("Convert pi to string using std::to_string", []() {
    return std::to_string(std::numbers::pi);
  });
  time_it("Convert pi to string using bee::to_string", []() {
    return bee::to_string(std::numbers::pi);
  });

  time_it("Convert pi*100000000 to int using bee::to_string", []() {
    return bee::to_string(int64_t(std::numbers::pi * 1000000000000000000ll));
  });
  time_it("Convert pi*100000000 to int using std::to_string", []() {
    return std::to_string(int64_t(std::numbers::pi * 1000000000000000000ll));
  });
}

int main()
{
  run_benchmark();
  ;
  return 0;
}

} // namespace
} // namespace bee

int main() { return bee::main(); }
