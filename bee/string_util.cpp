#include "bee/string_util.hpp"

#include <cassert>
#include <cstdint>
#include <optional>

#include "util.hpp"

namespace bee {
namespace {

std::vector<std::string> split_impl(
  const std::string_view& str,
  const std::string_view& sep,
  bool include_trailing_empty)
{
  assert(!sep.empty());
  std::vector<std::string> output;
  size_t pos = 0;
  while (1) {
    size_t next_pos = str.find(sep, pos);
    if (next_pos == std::string::npos) {
      if (pos != str.size() || include_trailing_empty) {
        output.push_back(std::string(str.substr(pos)));
      }
      break;
    }
    output.push_back(std::string(str.substr(pos, next_pos - pos)));
    pos = next_pos + sep.size();
  }
  return output;
}

} // namespace

std::vector<std::string> split(
  const std::string_view& str, const std::string_view& sep)
{
  return split_impl(str, sep, true);
}

std::vector<std::string> split_space(
  const std::string_view& str, const int max_parts)
{
  std::vector<std::string> output;
  std::string partial;

  auto flush = [&]() {
    if (!partial.empty()) {
      output.push_back(partial);
      partial.clear();
    }
  };

  for (char c : str) {
    if (isspace(c)) {
      if (std::ssize(output) + 2 <= max_parts || partial.empty()) {
        flush();
        continue;
      }
    }
    partial += c;
  }
  flush();

  return output;
}

std::vector<std::string> split_lines(const std::string_view& str)
{
  return split_impl(str, "\n", false);
}

std::string right_pad_string(std::string str, size_t length)
{
  while (str.size() < length) { str += ' '; }
  return str;
}

std::optional<std::string> remove_prefix(
  const std::string_view& str, const std::string_view& prefix)
{
  if (!str.starts_with(prefix)) { return std::nullopt; }
  return std::string(str.substr(prefix.size()));
}

std::optional<std::string> remove_suffix(
  const std::string_view& str, const std::string_view& suffix)
{
  if (!str.ends_with(suffix)) { return std::nullopt; }
  return std::string(str.substr(0, str.size() - suffix.size()));
}

std::string find_and_replace(
  const std::string_view& str,
  const std::string_view& needle,
  const std::string_view& rep)
{
  std::string output;
  size_t pos = 0;

  while (1) {
    auto idx = std::min(str.size(), str.find(needle, pos));
    while (pos < idx) {
      output += str[pos];
      pos++;
    }
    if (pos == str.size()) { break; }
    output += rep;
    pos += needle.size();
  }

  return output;
}

std::string trim_spaces(const std::string_view& str)
{
  size_t begin;
  for (begin = 0; begin < str.size(); begin++) {
    if (!isspace(str[begin])) { break; }
  }

  size_t end;
  for (end = str.size(); end > 0; end--) {
    if (!isspace(str[end - 1])) { break; }
  }

  return std::string(str.substr(begin, end - begin));
}

bool contains_string(
  const std::string_view& str, const std::string_view& needle)
{
  return str.find(needle) != std::string::npos;
}

template <class F> std::string map_string(const std::string_view& str, F&& f)
{
  std::string out;
  for (const char c : str) { out.push_back(f(c)); }
  return out;
}

std::string to_lower(const std::string_view& str)
{
  return map_string(str, tolower);
}

std::string to_upper(const std::string_view& str)
{
  return map_string(str, toupper);
}

} // namespace bee
