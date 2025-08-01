#pragma once

#include "bee/print.hpp"

namespace bee {

struct test_info {
  void (*run)();
  const char* name;
};

void run_tests();

int add_to_tests(void (*run)(), const char* name);

#define TEST(name)                                                             \
  void test_##name();                                                          \
  namespace {                                                                  \
  int _add_##name = bee::add_to_tests(test_##name, #name);                     \
  }                                                                            \
  void test_##name()

#define PRINT_EXPR(v) P("$ -> '$'", #v, v);

} // namespace bee
