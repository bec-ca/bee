#include "reader.hpp"

namespace bee {

Reader::~Reader() noexcept {}

OrError<std::string> Reader::read_str(size_t size)
{
  std::string output;
  output.resize(size);
  bail(bytes_read, read_raw(reinterpret_cast<std::byte*>(output.data()), size));
  output.resize(bytes_read);
  return output;
}

OrError<size_t> Reader::read(Bytes& buffer, size_t size)
{
  buffer.resize(size);
  bail(bytes_read, read_raw(buffer.data(), size));
  buffer.resize(bytes_read);
  return bytes_read;
}

OrError<size_t> Reader::read(std::byte* buffer, size_t size)
{
  return read_raw(buffer, size);
}

} // namespace bee
