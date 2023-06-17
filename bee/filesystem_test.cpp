#include <chrono>
#include <filesystem>
#include <random>

#include "error.hpp"
#include "filesystem.hpp"
#include "format.hpp"
#include "format_filesystem.hpp"
#include "format_vector.hpp"
#include "sort.hpp"
#include "string_util.hpp"
#include "testing.hpp"

namespace bee {
namespace {

FilePath create_tmp_dir()
{
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> dist;
  auto tmp_dir = FilePath::of_std_path(fs::temp_directory_path()) /
                 F("filesystem-test-$", dist(rd));
  must_unit(FileSystem::mkdirs(tmp_dir));
  return tmp_dir;
}

FilePath remove_path_prefix(const FilePath& p, const FilePath& prefix)
{
  return FilePath::of_string(remove_prefix(p.to_string(), prefix.to_string()));
}

TEST(mkdir)
{
  auto tmp_dir = create_tmp_dir();

  must_unit(FileSystem::mkdirs(tmp_dir));
  must_unit(FileSystem::touch_file(tmp_dir / "file.txt"));

  must(files, FileSystem::list_regular_files(tmp_dir));
  for (const auto& file : files) {
    P("File: $", remove_path_prefix(file, tmp_dir));
  }

  fs::remove_all(tmp_dir.to_std_path());
}

TEST(list_files)
{
  auto tmp_dir = create_tmp_dir();

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
    must(files, FileSystem::list_regular_files(tmp_dir, {.recursive = true}));
    sort(files);
    for (const auto& file : files) {
      P("File: $", remove_path_prefix(file, tmp_dir));
    }
  }

  {
    fs::current_path(tmp_dir.to_std_path());
    must(
      files,
      FileSystem::list_regular_files(
        FilePath::of_string("./"), {.recursive = true}));
    sort(files);
    for (const auto& file : files) { P("File: $", file); }
  }

  fs::remove_all(tmp_dir.to_std_path());
}

TEST(mtime)
{
  auto tmp_dir = create_tmp_dir();
  auto file_name = tmp_dir / "file.txt";

  must_unit(FileSystem::touch_file(file_name));

  must(mtime, FileSystem::file_mtime(file_name));
  P(mtime > Time::epoch());

  fs::remove_all(tmp_dir.to_std_path());
}

} // namespace
} // namespace bee
