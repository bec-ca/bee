#include "error.hpp"

#include "format.hpp"

namespace bee {

using std::deque;
using std::nullopt;
using std::string;

Error::Error(const Error& error) = default;
Error::Error(Error&& error) noexcept = default;

Error& Error::operator=(const Error& error) = default;
Error& Error::operator=(Error&& error) noexcept = default;

Error::Error(const string& msg)
    : _messages({{.message = msg, .location = nullopt}})
{}

Error::Error(const char* msg)
    : _messages({{.message = msg, .location = nullopt}})
{}

Error::Error(string&& msg)
    : _messages({{.message = std::move(msg), .location = nullopt}})
{}

Error::Error(const bee::Exn& exn)
    : _messages(
        {{.message = string("Exn raised: ") + exn.what(),
          .location = exn.loc()}})
{}

Error::Error(const std::exception& exn)
    : _messages(
        {{.message = string("Exn raised: ") + exn.what(), .location = nullopt}})
{}

Error::Error(const Location& loc, const string& msg)
    : _messages({{.message = msg, .location = loc}})
{}

Error::Error(const Location& loc, string&& msg)
    : _messages({{.message = std::move(msg), .location = loc}})
{}

Error::~Error() noexcept {}

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
      out +=
        F("$:$:$\n", msg.location->filename, msg.location->line, msg.message);
    } else {
      out += F("$\n", msg.message);
    }
  }
  return out;
}

const deque<ErrorMessage>& Error::messages() const { return _messages; }

void Error::raise() const { throw(bee::Exn(full_msg())); }

void Error::add_tag(string&& msg)
{
  _messages.emplace_front(
    ErrorMessage{.message = std::move(msg), .location = nullopt});
}

void Error::add_tag(const string& msg) { add_tag(string(msg)); }

void Error::add_tag(const char* tag) { add_tag(string(tag)); }

void Error::add_tag_with_location(const Location& loc, string&& msg)
{
  _messages.emplace_front(
    ErrorMessage{.message = std::move(msg), .location = loc});
}

void Error::add_tag_with_location(const Location& loc, const string& msg)
{
  add_tag_with_location(loc, string(msg));
}

void Error::add_tag_with_location(const Location& loc, const char* msg)
{
  add_tag_with_location(loc, string(msg));
}

} // namespace bee
