#include "util.hpp"

#include "format.hpp"
#include "string_util.hpp"
#include "testing.hpp"

using std::string;

namespace bee {
namespace {

TEST(tuple_to_vector)
{
  string a = "foo";
  string b = "bar";
  auto tuple = make_tuple(a, b);
  auto v = tuple_to_vector<string>(tuple);
  print_line(join(v, " "));
}

TEST(is_one_of)
{
  PRINT_EXPR((is_one_of_v<int, int>));
  PRINT_EXPR((is_one_of_v<float, int>));
  PRINT_EXPR((is_one_of_v<float, float>));
  PRINT_EXPR((is_one_of_v<int, string, float, int>));
  PRINT_EXPR((is_one_of_v<int, string, float, char>));
  PRINT_EXPR((is_one_of_v<int, string, float, int*>));
  PRINT_EXPR((is_one_of_v<string&, int, string>));
}

} // namespace
} // namespace bee
