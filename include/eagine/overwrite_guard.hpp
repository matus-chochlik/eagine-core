/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_OVERWRITE_GUARD_HPP
#define EAGINE_OVERWRITE_GUARD_HPP

#include "assert.hpp"
#include "config/basic.hpp"
#include <cstdint>

namespace eagine {

#if EAGINE_LOW_PROFILE
template <std::uint64_t = 0>
struct overwrite_guard {};
#else
template <std::uint64_t Token = 0xde7ec7ab1eU>
class overwrite_guard {
public:
    overwrite_guard() noexcept = default;
    overwrite_guard(overwrite_guard&&) noexcept = default;
    overwrite_guard(const overwrite_guard&) noexcept = default;
    auto operator=(overwrite_guard&&) noexcept -> overwrite_guard& = default;
    auto operator=(const overwrite_guard&) noexcept
      -> overwrite_guard& = default;
    ~overwrite_guard() noexcept {
        EAGINE_ASSERT(_token == Token);
    }

private:
    std::uint64_t _token{Token};
};
#endif

} // namespace eagine

#endif // EAGINE_OVERWRITE_GUARD_HPP
