#include "error.hpp"
#include "util.hpp"

#include <cassert>
#include <optional>

using std::string;
using std::vector;

namespace bee {

vector<string> split(const string& str, const string& sep)
{
  assert(!sep.empty());
  vector<string> output;
  size_t pos = 0;
  while (1) {
    size_t next_pos = str.find(sep, pos);
    if (next_pos == string::npos) {
      output.push_back(str.substr(pos));
      break;
    }
    output.push_back(str.substr(pos, next_pos - pos));
    pos = next_pos + sep.size();
  }
  return output;
}

vector<string> split_space(const string& str)
{
  vector<string> output;
  string partial;

  auto flush = [&]() {
    if (!partial.empty()) {
      output.push_back(partial);
      partial.clear();
    }
  };

  for (char c : str) {
    if (isspace(c)) {
      flush();
    } else {
      partial += c;
    }
  }
  flush();

  return output;
}

string right_pad_string(string str, size_t length)
{
  while (str.size() < length) { str += ' '; }
  return str;
}

bool ends_with(string const& value, string const& ending)
{
  if (ending.size() > value.size()) return false;
  return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

string to_hex(const string& str)
{
  auto hex_digit = [](int value) {
    if (value < 10) return value + '0';
    return value - 10 + 'a';
  };
  string out;
  for (uint8_t c : str) {
    string k(2, ' ');
    k[0] = hex_digit(c / 0x10);
    k[1] = hex_digit(c % 0x10);
    out += k;
  }
  return out;
}

OrError<string> of_hex(const string& str)
{
  if (str.size() % 2 != 0) {
    return Error("Hex string must have even number of characters");
  }

  auto of_hex_digit = [](char d) -> std::optional<uint8_t> {
    if (d >= '0' && d <= '9') return d - '0';
    if (d >= 'a' && d <= 'f') return d - 'a' + 10;
    if (d >= 'A' && d <= 'F') return d - 'A' + 10;
    return std::nullopt;
  };

  string out;
  for (size_t i = 0; i < str.size(); i += 2) {
    auto d1 = of_hex_digit(str[i]);
    auto d2 = of_hex_digit(str[i + 1]);
    if (!d1.has_value() || !d2.has_value()) {
      return Error("Not a valid hex string");
    }
    uint8_t v = *d1 * 16 + *d2;
    out.push_back(v);
  }
  return out;
}

std::string remove_prefix(const std::string& str, const std::string& prefix)
{
  if (!str.starts_with(prefix)) { return str; }
  return str.substr(prefix.size());
}

} // namespace bee
