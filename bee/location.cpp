#include "location.hpp"

#include "to_string.hpp"

namespace bee {

std::string Location::to_string() const
{
  return std::string(filename) + ":" + bee::to_string(line);
}

} // namespace bee
