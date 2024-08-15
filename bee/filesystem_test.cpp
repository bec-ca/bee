#include <chrono>
#include <filesystem>
#include <random>

#include "filesystem.hpp"
#include "format.hpp"
#include "format_vector.hpp"
#include "scoped_tmp_dir.hpp"
#include "sort.hpp"
#include "string_util.hpp"
#include "testing.hpp"

namespace bee {
namespace {

ScopedTmpDir create_tmp_dir()
{
  must(out, ScopedTmpDir::create());
  return std::move(out);
}

FilePath remove_path_prefix(const FilePath& p, const FilePath& prefix)
{
  return FilePath(remove_prefix(p.to_string(), prefix.to_string()).value());
}

TEST(mkdir)
{
  auto scoped_tmp_dir = create_tmp_dir();
  auto tmp_dir = scoped_tmp_dir.path();

  must_unit(FileSystem::mkdirs(tmp_dir));
  must_unit(FileSystem::touch_file(tmp_dir / "file.txt"));

  must(files, FileSystem::list_regular_files(tmp_dir));
  for (const auto& file : files) {
    P("File: $", remove_path_prefix(file, tmp_dir));
  }
}

TEST(list_files)
{
  auto scoped_tmp_dir = create_tmp_dir();
  auto tmp_dir = scoped_tmp_dir.path();

  must_unit(FileSystem::mkdirs(tmp_dir / "dir1"));
  must_unit(FileSystem::mkdirs(tmp_dir / "dir2"));
  must_unit(FileSystem::mkdirs(tmp_dir / "dir3"));

  must_unit(FileSystem::mkdirs(tmp_dir / "dir1/sub1"));
  must_unit(FileSystem::mkdirs(tmp_dir / "dir1/sub2"));
  must_unit(FileSystem::mkdirs(tmp_dir / "dir1/sub3"));

  must_unit(FileSystem::touch_file(tmp_dir / "dir1/sub1/file1.txt"));
  must_unit(FileSystem::touch_file(tmp_dir / "dir1/sub2/file2.txt"));
  must_unit(FileSystem::touch_file(tmp_dir / "dir2/file3.txt"));
  must_unit(FileSystem::touch_file(tmp_dir / "file4.txt"));

  {
    P("----");
    must(files, FileSystem::list_regular_files(tmp_dir, {.recursive = true}));
    sort(files);
    for (const auto& file : files) {
      P("File: $", remove_path_prefix(file, tmp_dir));
    }
  }

  {
    P("----");
    must_unit(FileSystem::set_current_dir(tmp_dir));
    must(
      files,
      FileSystem::list_regular_files(FilePath("./"), {.recursive = true}));
    sort(files);
    for (const auto& file : files) { P("File: $", file); }
  }
}

TEST(list_relative_path)
{
  auto scoped_tmp_dir = create_tmp_dir();
  auto root_dir = scoped_tmp_dir.path();

  must_unit(FileSystem::set_current_dir(root_dir));

  FilePath base_dir("base");

  must_unit(FileSystem::mkdirs(base_dir / "dir1"));
  must_unit(FileSystem::mkdirs(base_dir / "dir2"));
  must_unit(FileSystem::mkdirs(base_dir / "dir3"));

  must_unit(FileSystem::mkdirs(base_dir / "dir1/sub1"));
  must_unit(FileSystem::mkdirs(base_dir / "dir1/sub2"));
  must_unit(FileSystem::mkdirs(base_dir / "dir1/sub3"));

  must_unit(FileSystem::touch_file(base_dir / "dir1/sub1/file1.txt"));
  must_unit(FileSystem::touch_file(base_dir / "dir1/sub2/file2.txt"));
  must_unit(FileSystem::touch_file(base_dir / "dir2/file3.txt"));
  must_unit(FileSystem::touch_file(base_dir / "file4.txt"));

  auto run_test = [&](bool relative) {
    P("----");
    P("Relative: $", relative);
    must(
      files,
      FileSystem::list_regular_files(
        base_dir, {.recursive = true, .relative_path = relative}));
    sort(files);
    for (const auto& file : files) { P("File: $", file); }
  };

  run_test(true);
  run_test(false);
}

TEST(mtime)
{
  auto scoped_tmp_dir = create_tmp_dir();
  auto tmp_dir = scoped_tmp_dir.path();
  auto file_name = tmp_dir / "file.txt";

  must_unit(FileSystem::touch_file(file_name));

  must(mtime, FileSystem::file_mtime(file_name));
  P(mtime > Time::epoch());
}

TEST(create_temp_dir)
{
  must(path, FileSystem::create_temp_dir(FilePath("/tmp/bee-test-")));
  must_unit(FileSystem::touch_file(path / "foo"));
  P(FileSystem::exists(path / "foo"));
  P(FileSystem::exists(path / "other"));
  must_unit(FileSystem::remove_all(path));
}

} // namespace
} // namespace bee
