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

export struct spinlock_stats {
public:
    void locked() noexcept {
        ++_lock_count;
    }

    void yielded() noexcept {
        ++_yield_count;
    }

    [[nodiscard]] auto lock_count() const noexcept -> std::uintmax_t {
        return _lock_count;
    }

    [[nodiscard]] auto yield_count() const noexcept -> std::uintmax_t {
        return _yield_count;
    }

private:
    std::uintmax_t _lock_count{0U};
    std::uintmax_t _yield_count{0U};
};

/// @brief Spinlock drop-in replacement for std mutex.
/// @ingroup utility
export template <unsigned yieldModulo>
class basic_spinlock {
public:
    /// @brief Locks the spinlock.
    void lock() noexcept {
        unsigned i = 0;
        while(_flag.test() or _flag.test_and_set(std::memory_order_acquire)) {
            if((++i % yieldModulo) == 0U) {
                if constexpr(not low_profile_build) {
                    _stats.yielded();
                }
                std::this_thread::yield();
            }
        }
        if constexpr(not low_profile_build) {
            _stats.locked();
        }
    }

    /// @brief Unlocks the spinlock.
    void unlock() noexcept {
        _flag.clear(std::memory_order_release);
    }

    /// @brief Returns the number of times the spinlock was locked.
    [[nodiscard]] auto stats() const noexcept
      -> optional_reference_wrapper<const spinlock_stats> {
        if constexpr(not low_profile_build) {
            return {_stats};
        } else {
            return {nothing};
        }
    }

private:
    [[no_unique_address]] not_in_low_profile<spinlock_stats> _stats{};
    std::atomic_flag _flag{};
};

export using spinlock = basic_spinlock<32U>;

} // namespace eagine
