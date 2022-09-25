/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:spinlock;

import <thread>;
import <atomic>;

namespace eagine {

/// @brief Spinlock drop-in replacement for std mutex.
/// @ingroup utility
export class spinlock {
public:
    void lock() noexcept {
        unsigned i = 0;
        while(_flag.test() || _flag.test_and_set(std::memory_order_acquire)) {
            if((++i % 16U) == 0U) {
                std::this_thread::yield();
            }
        }
    }

    void unlock() noexcept {
        _flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag _flag{};
};

} // namespace eagine
