#include "bytes.hpp"

namespace bee {

Bytes::Bytes(const std::string& str)
{
  reserve(str.size());
  for (char c : str) { push_back(static_cast<std::byte>(c)); }
}

std::string Bytes::to_string() const
{
  std::string out;
  out.reserve(size());
  for (const auto b : *this) { out.push_back(static_cast<char>(b)); }
  return out;
}

} // namespace bee
