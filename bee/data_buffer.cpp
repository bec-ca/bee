#include "data_buffer.hpp"

#include <cassert>
#include <cstddef>

#include "bee/util.hpp"

using std::string;
using std::vector;

namespace bee {
namespace {

using Bytes = vector<std::byte>;

Bytes to_bytes(const string& str)
{
  auto bytes = reinterpret_cast<const std::byte*>(str.data());
  return Bytes(bytes, bytes + str.size());
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// DataBlock
//

DataBlock::DataBlock(Bytes&& data) : _data(std::move(data)), _start(0) {}
DataBlock::DataBlock(const Bytes& data) : _data(data), _start(0) {}

DataBlock::DataBlock(const std::byte* data, size_t size)
    : _data(data, data + size), _start(0)
{}

const std::byte* DataBlock::data() const { return _data.data() + _start; }
std::byte* DataBlock::data() { return _data.data() + _start; }

size_t DataBlock::size() const { return _data.size() - _start; }

bool DataBlock::empty() const { return size() == 0; }

const std::byte* DataBlock::begin() const { return _data.data() + _start; }
const std::byte* DataBlock::end() const { return begin() + size(); }

void DataBlock::consume(size_t bytes)
{
  assert(bytes <= size());
  _start += bytes;
}

std::byte DataBlock::read_byte()
{
  std::byte out = _data[_start];
  _start++;
  return out;
}

////////////////////////////////////////////////////////////////////////////////
// DataBuffer
//

DataBuffer::DataBuffer() {}
DataBuffer::DataBuffer(const string& data)
{
  _blocks.emplace_back(to_bytes(data));
}

DataBuffer::~DataBuffer() {}

bool DataBuffer::empty() const
{
  for (const auto& block : _blocks) {
    if (!block.empty()) return false;
  }
  return true;
}

size_t DataBuffer::size() const
{
  size_t size = 0;
  for (const auto& block : _blocks) { size += block.size(); }
  return size;
}

void DataBuffer::write(const string& data)
{
  _blocks.push_back(DataBlock(to_bytes(data)));
}

void DataBuffer::write(DataBuffer&& other)
{
  if (_blocks.empty()) {
    _blocks = std::move(other._blocks);
  } else {
    for (auto& block : other._blocks) { _blocks.push_back(std::move(block)); }
  }
  other.clear();
}

void DataBuffer::write(const std::byte* data, size_t size)
{
  _blocks.push_back(DataBlock(data, size));
}

void DataBuffer::write(std::vector<std::byte>&& data)
{
  _blocks.push_back(DataBlock(std::move(data)));
}

void DataBuffer::prepend(DataBuffer&& other)
{
  if (_blocks.empty()) {
    _blocks = std::move(other._blocks);
  } else {
    for (auto& block : rev_it(other._blocks)) {
      _blocks.push_front(std::move(block));
    }
  }
  other.clear();
}

string DataBuffer::to_string() const
{
  string output;
  for (auto& block : _blocks) {
    for (std::byte b : block) { output += std::to_integer<char>(b); }
  }
  return output;
}

string DataBuffer::read_string(size_t size)
{
  string output;
  for (auto& block : _blocks) {
    if (output.size() >= size) break;
    auto end = block.begin() + std::min(block.size(), size - output.size());
    for (auto it = block.begin(); it != end; it++) {
      output += std::to_integer<char>(*it);
    }
  }
  consume(output.size());
  return output;
}

std::optional<string> DataBuffer::read_line()
{
  string output;
  bool found_eol = false;
  auto read_until_eol = [&]() {
    for (auto& block : _blocks) {
      for (std::byte b : block) {
        char c = std::to_integer<char>(b);
        if (c == '\n') {
          found_eol = true;
          return;
        }
        output += c;
      }
    }
  };
  read_until_eol();
  if (!found_eol) { return std::nullopt; }
  consume(output.size() + 1);
  return output;
}

std::byte DataBuffer::read_byte()
{
  for (auto& block : _blocks) {
    if (block.empty()) continue;
    return block.read_byte();
  }
  assert(false && "Buffer is empty");
}

DataBlock& DataBuffer::top()
{
  assert(!empty());
  return _blocks.front();
}

void DataBuffer::consume(size_t bytes)
{
  while (bytes > 0) {
    assert(!_blocks.empty());
    auto& block = _blocks.front();
    size_t consume_in_block = std::min(bytes, block.size());
    block.consume(consume_in_block);
    bytes -= consume_in_block;
    if (block.empty()) { _blocks.pop_front(); }
  }
}

void DataBuffer::clear() { _blocks.clear(); }

std::deque<DataBlock>::const_iterator DataBuffer::begin() const
{
  return _blocks.begin();
}

std::deque<DataBlock>::const_iterator DataBuffer::end() const
{
  return _blocks.end();
}

std::deque<DataBlock>::iterator DataBuffer::begin() { return _blocks.begin(); }

std::deque<DataBlock>::iterator DataBuffer::end() { return _blocks.end(); }

} // namespace bee
