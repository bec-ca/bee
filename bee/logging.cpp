#include "logging.hpp"

#include <string>

#include "log_output.hpp"
#include "print.hpp"
#include "time.hpp"

namespace bee {
namespace {

LogOptions global_log_options{
  .output = LogOutput::StdErr,
  .include_time = true,
  .include_location = true,
};

}

namespace details {

void print_log(const Location& loc, const std::string& msg)
{
  std::string line;
  if (global_log_options.include_time) [[likely]] {
    line +=
      bee::Time::now().to_string_localtime({.exact_decimal_places = true});
    line += " | ";
  }

  if (global_log_options.include_location) [[likely]] {
    line += loc.to_string();
    line += " | ";
  }

  line += msg;

  PF(global_log_options.output, line);
}

} // namespace details

void set_log_options(const LogOptions& opts) { global_log_options = opts; }

} // namespace bee
