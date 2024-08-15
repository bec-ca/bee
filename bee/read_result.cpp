#include "read_result.hpp"

#include <cassert>

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// ReadResult
//

ReadResult::ReadResult(size_t bytes_read, bool is_eof)
    : _bytes_read(bytes_read), _is_eof(is_eof)
{
  assert(!_is_eof || _bytes_read == 0);
}

ReadResult::ReadResult(size_t bytes_read) : ReadResult(bytes_read, false) {}

ReadResult ReadResult::eof() { return ReadResult(0, true); }

bool ReadResult::is_eof() const { return _is_eof; }

size_t ReadResult::bytes_read() const { return _bytes_read; }

ReadResult ReadResult::empty() { return ReadResult(0); }

} // namespace bee
