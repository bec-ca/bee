#pragma once

#include <functional>
#include <string>
#include <vector>

#include "bee/format.hpp"

namespace bee {

struct test_info {
  std::function<void()> t;
  std::string name;
};

void run_tests();

int add_to_tests(std::function<void()> f, const std::string& name);

#define TEST(name)                                                             \
  void test_##name();                                                          \
  namespace {                                                                  \
  int _add_##name = bee::add_to_tests(test_##name, #name);                     \
  }                                                                            \
  void test_##name()

#define PRINT_EXPR(v) P("$ -> $", #v, v);

} // namespace bee
