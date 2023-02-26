#pragma once

#include "data_buffer.hpp"
#include "error.hpp"

namespace bee {

struct BinaryFormat {
  static void write_var_uint(DataBuffer& output, uint64_t value);
  static bee::OrError<uint64_t> read_var_uint(DataBuffer& buffer);

  static uint32_t read_uint32(DataBuffer& buffer);
  static void write_uint32(DataBuffer& buffer, uint32_t value);

  static uint64_t read_uint64(DataBuffer& buffer);
  static void write_uint64(DataBuffer& buffer, uint64_t value);

  static void write_size_and_string(DataBuffer& buffer, std::string str);
  static bee::OrError<std::string> read_size_and_string(DataBuffer& buffer);
};

} // namespace bee
