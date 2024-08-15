#include "file_path.hpp"
#include "testing.hpp"

namespace bee {
namespace {

TEST(relative_to)
{
  auto run_test = [](const char* s1, const char* s2) {
    auto p1 = FilePath(s1);
    auto p2 = FilePath(s2);
    P("$ $ -> $", s1, s2, p1.relative_to(p2));
  };

  run_test("foo", "bar");
  run_test("foo", "foo");
  run_test("foo", "foo/bar");
  run_test("foo/bar", "foo");
}

TEST(stem)
{
  auto run_test = [](const char* s) { P("'$' -> '$'", s, FilePath(s).stem()); };

  run_test("");
  run_test("foo");
  run_test("foo.txt");
  run_test("dir/foo.txt");
  run_test("/root/dir/foo.txt");
}

TEST(parent)
{
  auto run_test = [](const char* s) {
    P("--------");
    FilePath path{s};
    P(path);
    for (int i = 0; i < 10; i++) {
      auto parent = path.parent();
      if (parent == path) break;
      path = parent;
      P("path:$ empty:$", path, path.empty());
    }
  };
  run_test("/foo/bar/a/b/c");
  run_test("foo/bar/a/b/c");
}

TEST(empty)
{
  auto run_test = [](const char* s) { P("'$' -> $", s, FilePath(s).empty()); };
  run_test("/foo");
  run_test("/");
  run_test("");
}

} // namespace
} // namespace bee
