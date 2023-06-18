#include "exn.hpp"

namespace bee {

Exn::Exn(const Exn& other) noexcept = default;

Exn::~Exn() {}

const char* Exn::what() const noexcept { return _what.c_str(); }

const std::optional<Location>& Exn::loc() const noexcept { return _loc; }

} // namespace bee
