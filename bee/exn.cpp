#include "exn.hpp"

namespace bee {
namespace {

std::string make_format(const Location& loc, const std::string_view& what)
{
  auto out = loc.to_string();
  out += ":";
  out += what;
  return out;
}

} // namespace

Exn::Exn(const Exn& other) noexcept = default;
Exn::Exn(Exn&& other) noexcept = default;

Exn::Exn(const std::string_view what) : _what(what), _no_loc_what(what) {}
Exn::Exn(const Location& loc, const std::string_view what)
    : _loc(loc), _what(make_format(loc, what)), _no_loc_what(what)
{}

Exn::~Exn() noexcept {}

const char* Exn::what() const noexcept { return _what.c_str(); }

const std::string& Exn::no_loc_what() const noexcept { return _no_loc_what; }

const std::optional<Location>& Exn::loc() const noexcept { return _loc; }

const std::string& Exn::to_string() const { return _what; }

} // namespace bee
