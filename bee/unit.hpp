#include <string>

namespace bee {

struct Unit {
  std::string to_string() const { return {}; }
};

static const Unit unit;

} // namespace bee
