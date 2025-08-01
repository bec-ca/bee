#include "hash_file.hpp"

#include "file_reader.hpp"
#include "simple_checksum.hpp"

namespace bee {

bee::OrError<std::string> HashFile::compute_simple_hash(
  const FilePath& filename)
{
  std::byte buffer[2048];
  bail(file, bee::FileReader::open(filename));
  SimpleChecksum h;
  while (true) {
    bail(bytes_read, file->read(buffer, sizeof(buffer)));
    if (bytes_read == 0) { break; }
    h.add_string(buffer, bytes_read);
  }
  return h.hex();
}

} // namespace bee
