#pragma once

#include "location.hpp"
#include "log_output.hpp"

namespace bee {
namespace details {

void print_log(const Location& loc, const std::string& msg);

} // namespace details

struct LogOptions {
  bee::LogOutput output;
  bool include_time;
  bool include_location;
};

void set_log_options(const LogOptions& opts);

#define LOG(args...) ::bee::details::print_log(HERE, F(args));

#define LOG_IF_ERROR(arg, msg...)                                              \
  do {                                                                         \
    auto var = (arg);                                                          \
    if (var.is_error()) {                                                      \
      var.error().add_tag(bee::maybe_format(msg));                             \
      LOG(var.error().full_msg());                                             \
    }                                                                          \
  } while (false)

} // namespace bee
