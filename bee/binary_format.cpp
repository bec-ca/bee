#include "binary_format.hpp"

#include "bee/data_buffer.hpp"

#include <cstddef>

using std::string;

namespace bee {

namespace {

template <class T> T read_uint(DataBuffer& buffer)
{
  assert(buffer.size() >= sizeof(T));
  T output = 0;

  int offset = 0;
  for (size_t i = 0; i < sizeof(T); i++) {
    T c1 = std::to_integer<T>(buffer.read_byte());
    output |= c1 << offset;
    offset += 8;
  }
  return output;
}

template <class T> void write_uint(DataBuffer& buffer, T value)
{
  string data;
  for (size_t i = 0; i < sizeof(T); i++) {
    uint8_t c = value & 0xff;
    value >>= 8;
    data.push_back(c);
  }
  buffer.write(std::move(data));
}

} // namespace

void BinaryFormat::write_var_uint(DataBuffer& buffer, uint64_t value)
{
  string data;
  do {
    uint8_t c = value & ((1 << 7) - 1);
    value >>= 7;
    if (value > 0) { c |= 1 << 7; }
    data.push_back(c);
  } while (value > 0);
  buffer.write(std::move(data));
}

OrError<uint64_t> BinaryFormat::read_var_uint(DataBuffer& buffer)
{
  uint64_t output = 0;
  int shift = 0;
  while (true) {
    if (buffer.empty()) {
      return Error("Unexpected end of buffer when reading varint");
    }
    uint8_t c = std::to_integer<uint8_t>(buffer.read_byte());
    uint64_t v = (c & ((1 << 7) - 1));
    // TODO: check for overflow
    output += v << shift;
    shift += 7;
    if ((v ^ c) == 0) break;
  }
  return output;
}

uint32_t BinaryFormat::read_uint32(DataBuffer& buffer)
{
  return read_uint<uint32_t>(buffer);
}

void BinaryFormat::write_uint32(DataBuffer& buffer, uint32_t value)
{
  return write_uint<uint32_t>(buffer, value);
}

uint64_t BinaryFormat::read_uint64(DataBuffer& buffer)
{
  return read_uint<uint64_t>(buffer);
}

void BinaryFormat::write_uint64(DataBuffer& buffer, uint64_t value)
{
  return write_uint<uint64_t>(buffer, value);
}

void BinaryFormat::write_size_and_string(DataBuffer& buffer, string str)
{
  write_var_uint(buffer, str.size());
  buffer.write(std::move(str));
}

OrError<string> BinaryFormat::read_size_and_string(DataBuffer& buffer)
{
  bail(size, read_var_uint(buffer));
  return buffer.read_string(size);
}

} // namespace bee
