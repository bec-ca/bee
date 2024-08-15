#include "errno_msg.hpp"

#include <cerrno>
#include <cstring>

namespace bee {

const char* errno_msg() { return strerror(errno); }

} // namespace bee
