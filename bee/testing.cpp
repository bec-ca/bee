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
    P("No tests found");
  } else {
    for (const auto& t : tests_singleton()) {
      P("=================================================================="
        "==============");
      P("Test: $", t.name);
      try {
        t.t();
      } catch (const std::exception& exn) {
        P("Exception raised by test: $", exn.what());
      }
      P("");
    }
  }
}

} // namespace bee

int main()
{
  bee::run_tests();
  return 0;
}
