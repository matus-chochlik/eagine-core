/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<systemd/sd-daemon.h>)
#include <systemd/sd-daemon.h>
#ifndef EAGINE_USE_SYSTEMD
#define EAGINE_USE_SYSTEMD 1
#endif
#else
#ifndef EAGINE_USE_SYSTEMD
#define EAGINE_USE_SYSTEMD 0
#endif
#endif

module eagine.core.main_ctx;

import eagine.core.types;
import eagine.core.utility;
import <chrono>;
import <cstdint>;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
#if EAGINE_USE_SYSTEMD
class process_watchdog_impl {
public:
    process_watchdog_impl() {
        std::uint64_t _interval_us{0};
        if(sd_watchdog_enabled(0, &_interval_us) > 0) {
            _should_notify.reset(
              std::chrono::microseconds(_interval_us / 2), nothing);
            _enabled = true;
        }
    }

    void ready() noexcept {
        if(_enabled) {
            ::sd_notify(0, "READY=1");
        }
    }

    void watchdog() noexcept {
        if(_enabled) {
            if(_should_notify) {
                ::sd_notify(0, "WATCHDOG=1");
            }
        }
    }

    void stopping() noexcept {
        if(_enabled) {
            ::sd_notify(0, "STOPPING=1");
        }
    }

private:
    resetting_timeout _should_notify;
    bool _enabled{false};
};
#endif
//------------------------------------------------------------------------------
auto process_watchdog::_impl() noexcept -> process_watchdog_impl* {
#if EAGINE_USE_SYSTEMD
    if(not _pimpl) [[unlikely]] {
        try {
            _pimpl = std::make_shared<process_watchdog_impl>();
        } catch(...) {
        }
    }
    return _pimpl.get();
#endif
    return nullptr;
}
//------------------------------------------------------------------------------
void process_watchdog::declare_initialized() noexcept {
#if EAGINE_USE_SYSTEMD
    if(auto impl{_impl()}) {
        extract(impl).ready();
    }
#endif
}
//------------------------------------------------------------------------------
void process_watchdog::notify_alive() noexcept {
#if EAGINE_USE_SYSTEMD
    if(auto impl{_impl()}) {
        extract(impl).watchdog();
    }
#endif
}
//------------------------------------------------------------------------------
void process_watchdog::announce_shutdown() noexcept {
#if EAGINE_USE_SYSTEMD
    if(auto impl{_impl()}) {
        extract(impl).stopping();
    }
#endif
}
//------------------------------------------------------------------------------
} // namespace eagine
