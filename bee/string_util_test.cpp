#include "format.hpp"
#include "format_optional.hpp"
#include "format_vector.hpp"
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
  split_and_print("foo bar yo ", " ");
  split_and_print("foobaryo", " ");
  split_and_print("", " ");
  split_and_print("foo  bar foobar", " ");
  split_and_print("foo  bar foobar", "  ");
  split_and_print("usr/bin/path/there", "/");
  split_and_print("/usr/bin/path/there/", "/");
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

TEST(find_and_replace)
{
  PRINT_EXPR(find_and_replace("foo.bar.baz", ".", "/"));
  PRINT_EXPR(find_and_replace(".foo..bar...baz....", "..", "/"));
}

TEST(trim_spaces)
{
  PRINT_EXPR(trim_spaces(" foo "));
  PRINT_EXPR(trim_spaces("   foo   "));
  PRINT_EXPR(trim_spaces(" \t  foo \n  "));
}

TEST(split_lines)
{
  PRINT_EXPR(split_lines("foo"));
  PRINT_EXPR(split_lines("foo\n"));
  PRINT_EXPR(split_lines("foo\nbar"));
  PRINT_EXPR(split_lines("foo\nbar\n"));
}

TEST(lower_upper)
{
  PRINT_EXPR(to_upper("foo"));
  PRINT_EXPR(to_upper("Foo"));
  PRINT_EXPR(to_upper("FOO"));

  PRINT_EXPR(to_lower("foo"));
  PRINT_EXPR(to_lower("Foo"));
  PRINT_EXPR(to_lower("FOO"));
}

TEST(remove_suffix)
{
  PRINT_EXPR(remove_suffix("foo", "bar"));
  PRINT_EXPR(remove_suffix("foobar", "bar"));
  PRINT_EXPR(remove_suffix("foobar", "foo"));
}

TEST(remove_prefix)
{
  PRINT_EXPR(remove_prefix("foo", "bar"));
  PRINT_EXPR(remove_prefix("foobar", "bar"));
  PRINT_EXPR(remove_prefix("foobar", "foo"));
}

} // namespace
} // namespace bee
