#include "format.hpp"
#include "format_vector.hpp"
#include "string_util.hpp"
#include "testing.hpp"
#include "util.hpp"

using std::string;

namespace bee {
namespace {

TEST(tuple_to_vector)
{
  string a = "foo";
  string b = "bar";
  auto tuple = make_tuple(a, b);
  auto v = tuple_to_vector<string>(tuple);
  P(join(v, " "));
}

TEST(is_one_of)
{
  PRINT_EXPR((is_one_of<int, int>));
  PRINT_EXPR((is_one_of<float, int>));
  PRINT_EXPR((is_one_of<float, float>));
  PRINT_EXPR((is_one_of<int, string, float, int>));
  PRINT_EXPR((is_one_of<int, string, float, char>));
  PRINT_EXPR((is_one_of<int, string, float, int*>));
  PRINT_EXPR((is_one_of<string&, int, string>));
}

struct Dummy {
 public:
  Dummy() {}
  Dummy(const Dummy&) noexcept : copy_constructed(true) {}
  Dummy(Dummy&&) noexcept : move_constructed(true) {}

  const char* to_string() const
  {
    if (copy_constructed) {
      return "copied";
    } else if (move_constructed) {
      return "moved";
    } else {
      return "neither";
    }
  }

 private:
  bool copy_constructed = false;
  bool move_constructed = false;
  std::string _dum;
};

TEST(concat_copy)
{
  std::vector<Dummy> values;
  values.emplace_back();
  values.emplace_back();
  values.emplace_back();
  P("values:$", values);

  std::vector<Dummy> copy = values;
  P("copy:$", copy);

  {
    std::vector<Dummy> moved = std::move(values);
    P("moved:$", moved);
  }

  {
    std::vector<Dummy> concated;
    concated.reserve(3);
    concat(concated, copy);
    P("concated:$", concated);
  }

  {
    std::vector<Dummy> concated_moved;
    concated_moved.reserve(3);
    concat(concated_moved, std::move(copy));
    P("concated_moved:$", concated_moved);
  }
}

} // namespace
} // namespace bee
