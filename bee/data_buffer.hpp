#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

namespace bee {

struct DataBlock {
 public:
  explicit DataBlock(std::vector<std::byte>&& data);
  explicit DataBlock(const std::vector<std::byte>& data);
  explicit DataBlock(const std::byte* data, size_t size);

  const std::byte* data() const;
  std::byte* data();

  size_t size() const;
  bool empty() const;

  std::string to_string() const;

  void consume(size_t bytes);

  std::byte read_byte();

  const std::byte* begin() const;
  const std::byte* end() const;

 private:
  std::vector<std::byte> _data;
  size_t _start;
};

struct DataBuffer {
 public:
  DataBuffer();
  explicit DataBuffer(const std::string& data);

  ~DataBuffer();

  void write(const std::string& data);
  void write(DataBuffer&& data);
  void write(const char* data, size_t size);
  void write(const std::byte* data, size_t size);
  void write(std::vector<std::byte>&& data);

  void prepend(DataBuffer&& data);

  std::byte read_byte();

  std::string to_string() const;

  std::string read_string(size_t bytes);

  std::optional<std::string> read_line();

  size_t size() const;

  bool empty() const;

  DataBlock& top();

  void consume(size_t bytes);

  void clear();

  std::deque<DataBlock>::const_iterator begin() const;
  std::deque<DataBlock>::const_iterator end() const;

  std::deque<DataBlock>::iterator begin();
  std::deque<DataBlock>::iterator end();

 private:
  std::deque<DataBlock> _blocks;
};

} // namespace bee
