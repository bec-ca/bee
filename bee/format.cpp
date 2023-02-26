#include "format.hpp"

#include <iostream>
#include <sstream>
#include <syncstream>

using std::cerr;
using std::cout;
using std::flush;
using std::string;

namespace bee {

void print_str(string str)
{
  str += '\n';
  cout << str << flush;
}

void print_err_str(string str)
{
  str += '\n';
  cerr << str << flush;
}

std::string to_string<void*>::convert(const void* value)
{
  return to_string<const void*>::convert(value);
}

std::string to_string<const void*>::convert(const void* value)
{
  std::ostringstream ss;
  ss << std::hex << value;
  return ss.str();
}

} // namespace bee
