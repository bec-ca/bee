#include "reader.hpp"

namespace bee {

Reader::~Reader() noexcept {}

OrError<std::string> Reader::read_str(const size_t size)
{
  if (size == 0) return {};
  std::string output;
  output.resize(size);
  bail(bytes_read, read_raw(reinterpret_cast<std::byte*>(output.data()), size));
  output.resize(bytes_read);
  return output;
}

OrError<std::vector<std::byte>> Reader::read_bytes(const size_t size)
{
  if (size == 0) return {};
  std::vector<std::byte> output;
  output.resize(size);
  bail(bytes_read, read_raw(output.data(), size));
  output.resize(bytes_read);
  return output;
}

OrError<size_t> Reader::read(Bytes& buffer, const size_t size)
{
  if (size == 0) return 0;

  buffer.resize(size);
  bail(bytes_read, read_raw(buffer.data(), size));
  buffer.resize(bytes_read);
  return bytes_read;
}

OrError<size_t> Reader::read(std::byte* const buffer, const size_t size)
{
  if (size == 0) return 0;

  return read_raw(buffer, size);
}

} // namespace bee
