/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:spinlock;

import eagine.core.build_config;
import eagine.core.types;
import <atomic>;
import <cstdint>;
import <optional>;
import <thread>;

namespace eagine {

/// @brief Spinlock drop-in replacement for std mutex.
/// @ingroup utility
export class spinlock {
public:
    /// @brief Locks the spinlock.
    void lock() noexcept {
        unsigned i = 0;
        while(_flag.test() || _flag.test_and_set(std::memory_order_acquire)) {
            if((++i % 16U) == 0U) {
                ++_yield_count;
                std::this_thread::yield();
            }
        }
    }

    /// @brief Unlocks the spinlock.
    void unlock() noexcept {
        _flag.clear(std::memory_order_release);
    }

    /// @brief Returns the number of times the spinlock was locked.
    auto yield_count() const noexcept -> std::optional<std::uintmax_t> {
        if constexpr(low_profile_build) {
            return {};
        } else {
            return {_yield_count};
        };
    }

private:
    [[no_unique_address]] not_in_low_profile<std::uintmax_t> _yield_count{0U};
    std::atomic_flag _flag{};
};

} // namespace eagine
