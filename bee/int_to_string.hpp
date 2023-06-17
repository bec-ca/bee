#include <string>

#include "format_params.hpp"
#include "to_string_t.hpp"

namespace bee {

template <> struct to_string_t<int> {
  static std::string convert(int value, const FormatParams& p);
};

template <> struct to_string_t<unsigned> {
  static std::string convert(unsigned value, const FormatParams& p);
};

template <> struct to_string_t<short> {
  static std::string convert(short value, const FormatParams& p);
};

template <> struct to_string_t<unsigned short> {
  static std::string convert(unsigned short value, const FormatParams& p);
};

template <> struct to_string_t<long> {
  static std::string convert(long value, const FormatParams& p);
};

template <> struct to_string_t<unsigned long> {
  static std::string convert(unsigned long value, const FormatParams& p);
};

template <> struct to_string_t<long long> {
  static std::string convert(long long value, const FormatParams& p);
};

template <> struct to_string_t<unsigned long long> {
  static std::string convert(unsigned long long value, const FormatParams& p);
};

} // namespace bee
