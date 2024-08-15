#include <string>

#include "format_params.hpp"
#include "to_string_t.hpp"

namespace bee {

#define DECLARE_CONVERTER(T)                                                   \
  template <> struct to_string_t<T> {                                          \
    static std::string convert(T value, const FormatParams& p);                \
  };

DECLARE_CONVERTER(signed char);
DECLARE_CONVERTER(unsigned char);
DECLARE_CONVERTER(short);
DECLARE_CONVERTER(unsigned short);
DECLARE_CONVERTER(int);
DECLARE_CONVERTER(unsigned);
DECLARE_CONVERTER(long);
DECLARE_CONVERTER(unsigned long);
DECLARE_CONVERTER(long long);
DECLARE_CONVERTER(unsigned long long);

#undef DECLARE_CONVERTER

} // namespace bee
