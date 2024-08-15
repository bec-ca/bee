#include "format.hpp"
#include "format_vector.hpp"
#include "parallel_map.hpp"
#include "sort.hpp"
#include "testing.hpp"
#include "util.hpp"

using std::make_pair;
using std::pair;
using std::vector;

namespace bee {
namespace {

bool check_prime(int number)
{
  if (number == 0 || number == 1) return false;
  if (number == 2) return true;
  if (number % 2 == 0) return false;

  for (int i = 3; i < number; i += 2) {
    if (number % i == 0) return false;
  }
  return true;
}

pair<int, bool> check_prime_runner(int number)
{
  return make_pair(number, check_prime(number));
}

TEST(basic)
{
  vector<int> inputs;
  vector<pair<int, bool>> outputs;
  for (int i = 5; i < 50; i++) { inputs.push_back(i); }
  for (auto r : ParallelMap::go(inputs, 2, check_prime_runner)) {
    outputs.push_back(r);
  }
  bee::sort(outputs);
  P(outputs);
}

} // namespace
} // namespace bee
