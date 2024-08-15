#include "exn.hpp"

namespace bee {
namespace {

std::string make_format(const Location& loc, const std::string_view& what)
{
  std::string out = loc.to_string() + ":";
  out += what;
  return out;
}

} // namespace

Exn::Exn(const Exn& other) noexcept = default;
Exn::Exn(Exn&& other) noexcept = default;

Exn::Exn(const std::string_view& what) : _what(what) {}
Exn::Exn(const Location& loc, const std::string_view& what)
    : _loc(loc), _what(make_format(loc, what))
{}

Exn::~Exn() noexcept {}

const char* Exn::what() const noexcept { return _what.c_str(); }

const std::optional<Location>& Exn::loc() const noexcept { return _loc; }

const std::string& Exn::to_string() const { return _what; }

} // namespace bee
