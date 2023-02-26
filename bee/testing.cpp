#include "testing.hpp"

#include "format.hpp"

using std::vector;

namespace bee {

vector<test_info>& tests_singleton()
{
  static vector<test_info> tests;
  return tests;
}

int add_to_tests(std::function<void()> f, const std::string& name)
{
  tests_singleton().push_back({f, name});
  return 0;
}

void run_tests()
{
  if (tests_singleton().empty()) {
    print_line("No tests found");
  } else {
    for (const auto& t : tests_singleton()) {
      print_line(
        "=================================================================="
        "==============");
      print_line("Test: $", t.name);
      t.t();
      print_line("");
    }
  }
}

} // namespace bee

int main()
{
  bee::run_tests();
  return 0;
}
