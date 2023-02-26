#include "error.hpp"

#include "format.hpp"

namespace bee {

using std::deque;
using std::nullopt;
using std::string;

Error::Error(const Error& error) = default;
Error::Error(Error&& error) = default;

Error& Error::operator=(const Error& error) = default;
Error& Error::operator=(Error&& error) = default;

Error::Error(const string& msg)
    : _messages({{.message = msg, .location = nullopt}})
{}

Error::Error(const char* msg)
    : _messages({{.message = msg, .location = nullopt}})
{}

Error::Error(string&& msg)
    : _messages({{.message = std::move(msg), .location = nullopt}})
{}

Error::Error(const char* filename, int line, const string& msg)
    : _messages({{
        .message = msg,
        .location =
          ErrorLocation{
            .filename = filename,
            .line = line,
          },
      }})
{}

Error::Error(const char* filename, int line, string&& msg)
    : _messages({{
        .message = std::move(msg),
        .location =
          ErrorLocation{
            .filename = filename,
            .line = line,
          },
      }})
{}

string Error::msg() const
{
  string out;
  for (const auto& msg : _messages) {
    if (msg.message.empty()) continue;
    if (!out.empty()) { out += ": "; }
    out += msg.message;
  }
  return out;
}

string Error::to_string() const { return msg(); }

string Error::full_msg() const
{
  string out;
  for (const auto& msg : _messages) {
    if (msg.location.has_value()) {
      out += bee::format(
        "$:$:$\n", msg.location->filename, msg.location->line, msg.message);
    } else {
      out += bee::format(":$\n", msg.message);
    }
  }
  return out;
}

void Error::print_error() const { print_err_line(full_msg()); }

const deque<ErrorMessage>& Error::messages() const { return _messages; }

void Error::crash() const
{
  print_err_line("Unhandled error:");
  print_err_line(full_msg());
  abort();
}

void Error::add_tag(string&& msg)
{
  _messages.emplace_front(
    ErrorMessage{.message = std::move(msg), .location = nullopt});
}

void Error::add_tag(const string& msg) { add_tag(string(msg)); }

void Error::add_tag(const char* tag) { add_tag(string(tag)); }

void Error::add_tag_with_location(const char* filename, int line, string&& msg)
{
  _messages.emplace_front(ErrorMessage{
    .message = std::move(msg),
    .location = ErrorLocation{.filename = filename, .line = line}});
}

void Error::add_tag_with_location(
  const char* filename, int line, const string& msg)
{
  add_tag_with_location(filename, line, string(msg));
}

void Error::add_tag_with_location(
  const char* filename, int line, const char* msg)
{
  add_tag_with_location(filename, line, string(msg));
}

} // namespace bee
