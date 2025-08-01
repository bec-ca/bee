#include "testing.hpp"

#include "file_writer.hpp"
#include "print.hpp"

namespace bee {

std::vector<test_info>& tests_singleton()
{
  static std::vector<test_info> tests;
  return tests;
}

int add_to_tests(void (*run)(), const char* name)
{
  tests_singleton().push_back({run, name});
  return 0;
}

void run_tests()
{
  FileWriter::stdout().set_buffered(false);
  if (tests_singleton().empty()) {
    P("No tests found");
  } else {
    for (const auto& t : tests_singleton()) {
      P("=================================================================="
        "==============");
      P("Test: $", t.name);
      t.run();
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
