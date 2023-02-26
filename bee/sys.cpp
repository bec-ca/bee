#include "sys.hpp"

#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

using bee::Time;
using std::string;

namespace bee {
namespace {

using stat_t = struct stat;

using timespec = struct timespec;

#ifdef __APPLE__

const timespec& timespec_mtime(const stat_t& s) { return s.st_mtimespec; }

#else

const timespec& timespec_mtime(const stat_t& s) { return s.st_mtim; }

#endif

Time time_of_timespec(const timespec& ts)
{
  return Time::of_timestamp_nanos(ts.tv_nsec + ts.tv_sec * 1000000000ll);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// mtime
//

bee::OrError<Time> Sys::file_mtime(const string& filename)
{
  struct stat statbuf;
  int code = stat(filename.c_str(), &statbuf);
  if (code != 0) {
    return bee::Error::format(
      "Failed to stat file $: $", filename, strerror(errno));
  }

  return time_of_timespec(timespec_mtime(statbuf));
}

} // namespace bee
