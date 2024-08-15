#include "writer.hpp"

#include <cstring>

namespace bee {

Writer::~Writer() noexcept {}

OrError<size_t> Writer::write(const std::string& data)
{
  return write(data.data(), data.size());
}

OrError<size_t> Writer::write(const std::vector<std::byte>& data)
{
  return write(data.data(), data.size());
}

OrError<size_t> Writer::write(const DataBuffer& data)
{
  size_t written = 0;
  for (const auto& block : data) {
    bail_unit(write(block.data(), block.size()));
    written += block.size();
  }
  return written;
}

OrError<size_t> Writer::write(const std::byte* data, size_t size)
{
  return write_raw(data, size);
}

OrError<size_t> Writer::write(const char* data, size_t size)
{
  return write_raw(reinterpret_cast<const std::byte*>(data), size);
}

OrError<size_t> Writer::write(const char* data)
{
  return write(data, strlen(data));
}

OrError<size_t> Writer::write_line(const std::string& data)
{
  bail(ret1, write(data.data(), data.size()));
  bail(ret2, write("\n"));
  return ret1 + ret2;
}

} // namespace bee
