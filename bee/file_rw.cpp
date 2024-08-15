#include "file_rw.hpp"

#include "fd.hpp"
#include "or_error.hpp"

namespace bee {
namespace {

// TODO: add a fd base class

struct FileRWImpl final : public FileRW {
 public:
  FileRWImpl(FD&& fd) : _fd(std::move(fd)) {}

  static OrError<ptr> open(const FilePath& filename)
  {
    bail(fd, FD::open_file(filename, FileMode::ReadWrite));
    return std::make_unique<FileRWImpl>(std::move(fd));
  }

  // Writer interface
  virtual OrError<size_t> write_raw(const std::byte* data, size_t size) override
  {
    return _fd.write(data, size);
  }

  // Reader interface
  virtual OrError<size_t> read_raw(std::byte* data, size_t size) override
  {
    bail(res, _fd.read(data, size));
    if (res.is_eof()) { return 0; }
    return res.bytes_read();
  }

  OrError<size_t> remaining_bytes() override { return _fd.remaining_bytes(); }

  // Closer interface
  virtual bool close() override { return _fd.close(); }

  OrError<> seek(size_t pos) { return _fd.seek(pos); }

 private:
  FD _fd;
};

} // namespace

OrError<FileRW::ptr> FileRW::open(const FilePath& filename)
{
  return FileRWImpl::open(filename);
}

} // namespace bee
