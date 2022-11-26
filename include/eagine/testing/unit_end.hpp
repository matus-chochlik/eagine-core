/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
import <iostream>;

namespace eagitest {
//------------------------------------------------------------------------------
// test suite
//------------------------------------------------------------------------------
suite::suite(std::string_view name) noexcept
  : _name{name} {}
//------------------------------------------------------------------------------
suite::~suite() noexcept {}
//------------------------------------------------------------------------------
auto suite::constructed(std::string_view, const auto&) noexcept -> suite& {
    return *this;
}
//------------------------------------------------------------------------------
auto suite::checkpoint(std::string_view) noexcept -> suite& {
    return *this;
}
//------------------------------------------------------------------------------
auto suite::operator()(void (*func)(eagitest::suite&)) -> suite& {
    func(*this);
    return *this;
}
//------------------------------------------------------------------------------
// test case
//------------------------------------------------------------------------------
case_::case_(std::string_view name, suite&) noexcept
  : _name{name} {}
//------------------------------------------------------------------------------
case_::~case_() noexcept {}
//------------------------------------------------------------------------------
} // namespace eagitest
