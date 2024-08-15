#pragma once

#include "fd.hpp"
#include "or_error.hpp"

namespace bee {

bee::OrError<FD> create_socket_fd(int family);

}
