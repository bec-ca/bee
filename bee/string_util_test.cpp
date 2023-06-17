#include "format.hpp"
#include "string_util.hpp"
#include "testing.hpp"

using std::string;
using std::vector;

namespace bee {
namespace {

void print_parts(vector<string> parts)
{
  for (auto& part : parts) { part = "'" + part + "'"; }
  P("num_parts:$ parts:[$]", parts.size(), join(parts, ","));
}

void split_and_print(const std::string& str, const std::string& sep)
{
  auto parts = split(str, sep);

  P("------------------------");
  P("str:'$' sep:'$'", str, sep);
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

    P("------------------------");
    P("'$'", str);
    print_parts(parts);
  };

  split_and_print("foo bar yo");
  split_and_print("   foo   bar    yo    ");
  split_and_print("    ");
  split_and_print("");
  split_and_print("   foo \n\t  bar   ");
}

TEST(split_space_max_parts)
{
  auto split_and_print = [](const std::string& str, int max_parts = 2) {
    auto parts = split_space(str, max_parts);

    P("------------------------");
    P("'$'", str);
    print_parts(parts);
  };

  split_and_print("foo bar yo");
  split_and_print("   foo   bar    yo    ");
  split_and_print("    ");
  split_and_print("");
  split_and_print("   foo \n\t  bar   ");
  split_and_print("foo bar yo baz taz");
  split_and_print("foo bar yo baz taz", 3);
  split_and_print("foo bar yo baz taz", 4);
}
} // namespace
} // namespace bee
