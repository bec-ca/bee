#include "testing.hpp"

#include "error.hpp"
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
      auto err = try_with([&]() { t.t(); });
      if (err.is_error()) {
        P("Exception raised by test");
        P(err.error().full_msg());
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
