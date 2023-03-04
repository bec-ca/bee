#include "filesystem.hpp"

#include "error.hpp"
#include "format.hpp"
#include "format_filesystem.hpp"
#include "format_vector.hpp"
#include "sort.hpp"
#include "string_util.hpp"
#include "testing.hpp"

#include <filesystem>
#include <random>

namespace bee {
namespace {

FilePath create_tmp_dir()
{
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> dist;
  auto tmp_dir = FilePath::of_std_path(fs::temp_directory_path()) /
                 format("filesystem-test-$", dist(rd));
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
    print_line("File: $", remove_path_prefix(file, tmp_dir));
  }
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
    must(files, FileSystem::list_regular_files(tmp_dir, true));
    sort(files);
    for (const auto& file : files) {
      print_line("File: $", remove_path_prefix(file, tmp_dir));
    }
  }

  {
    fs::current_path(tmp_dir.to_std_path());
    must(
      files, FileSystem::list_regular_files(FilePath::of_string("./"), true));
    sort(files);
    for (const auto& file : files) { print_line("File: $", file); }
  }
}

} // namespace
} // namespace bee
