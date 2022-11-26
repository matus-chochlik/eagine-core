/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
import <string_view>;

namespace eagitest {
//------------------------------------------------------------------------------
class suite {
public:
    suite(std::string_view name) noexcept;
    suite(suite&&) = delete;
    suite(const suite&) = delete;
    auto operator=(suite&&) = delete;
    auto operator=(const suite&) = delete;
    ~suite() noexcept;

    auto constructed(std::string_view, const auto&) noexcept -> suite&;

    auto checkpoint(std::string_view) noexcept -> suite&;

    auto operator()(void (*func)(eagitest::suite&)) -> suite&;

private:
    std::string_view _name;
};
//------------------------------------------------------------------------------
class case_ {
public:
    case_(std::string_view name, suite&) noexcept;
    case_(case_&&) = delete;
    case_(const case_&) = delete;
    auto operator=(case_&&) = delete;
    auto operator=(const case_&) = delete;
    ~case_() noexcept;

private:
    std::string_view _name;
};
//------------------------------------------------------------------------------
} // namespace eagitest

