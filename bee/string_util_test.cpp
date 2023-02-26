#include "string_util.hpp"

#include "format.hpp"
#include "testing.hpp"

using std::string;
using std::vector;

namespace bee {
namespace {

void print_parts(vector<string> parts)
{
  for (auto& part : parts) { part = "'" + part + "'"; }
  print_line("num_parts:$ parts:[$]", parts.size(), join(parts, ","));
}

void split_and_print(const std::string& str, const std::string& sep)
{
  auto parts = split(str, sep);

  print_line("------------------------");
  print_line("str:'$' sep:'$'", str, sep);
  print_parts(parts);
}

TEST(split)
{
  split_and_print("foo bar yo", " ");
  split_and_print("foobaryo", " ");
  split_and_print("", " ");
  split_and_print("foo  bar foobar", " ");
  split_and_print("foo  bar foobar", "  ");
  split_and_print("usr/bin/path/there", "/");
}

TEST(split_space)
{
  auto split_and_print = [](const std::string& str) {
    auto parts = split_space(str);

    print_line("------------------------");
    print_line("'$'", str);
    print_parts(parts);
  };

  split_and_print("foo bar yo");
  split_and_print("   foo   bar    yo    ");
  split_and_print("    ");
  split_and_print("");
  split_and_print("   foo \n\t  bar   ");
}

} // namespace
} // namespace bee
