#pragma once

namespace bee {

enum class OS {
  Linux,
  Macos,
};

#ifdef __APPLE__
constexpr OS RunningOS = OS::Macos;
#else
constexpr OS RunningOS = OS::Linux;
#endif

} // namespace bee
