/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

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

import std;
import eagine.core.types;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
struct process_watchdog_backend : interface<process_watchdog_backend> {
    virtual void declare_initialized() noexcept = 0;
    virtual void notify_alive() noexcept = 0;
    virtual void announce_shutdown() noexcept = 0;
};
//------------------------------------------------------------------------------
class null_process_watchdog_backend final : public process_watchdog_backend {
    void declare_initialized() noexcept final {}
    void notify_alive() noexcept final {}
    void announce_shutdown() noexcept final {}
};
//------------------------------------------------------------------------------
#if EAGINE_USE_SYSTEMD
class systemd_process_watchdog_backend final : public process_watchdog_backend {
public:
    systemd_process_watchdog_backend(std::chrono::microseconds interval) noexcept
      : _should_notify{interval, nothing} {}

    void declare_initialized() noexcept final {
        ::sd_notify(0, "READY=1");
    }

    void notify_alive() noexcept final {
        if(_should_notify) {
            ::sd_notify(0, "WATCHDOG=1");
        }
    }

    void announce_shutdown() noexcept final {
        ::sd_notify(0, "STOPPING=1");
    }

private:
    resetting_timeout _should_notify;
};
#endif
//------------------------------------------------------------------------------
static auto make_process_watchdog_backend() noexcept
  -> shared_holder<process_watchdog_backend> {
#if EAGINE_USE_SYSTEMD
    std::uint64_t _interval_us{0};
    if(sd_watchdog_enabled(0, &_interval_us) > 0) {
        return {
          hold<systemd_process_watchdog_backend>,
          std::chrono::microseconds{_interval_us / 2}};
    }
#endif
    return {hold<null_process_watchdog_backend>};
}
//------------------------------------------------------------------------------
process_watchdog::process_watchdog(main_ctx_parent parent) noexcept
  : main_ctx_object{"Watchdog", parent}
  , _backend{make_process_watchdog_backend()}
  , _should_log_heartbeat{std::chrono::seconds{30}, nothing} {}
//------------------------------------------------------------------------------
void process_watchdog::declare_initialized() noexcept {
    _backend->declare_initialized();
}
//------------------------------------------------------------------------------
void process_watchdog::notify_alive() noexcept {
    _backend->notify_alive();
    if(_should_log_heartbeat) [[unlikely]] {
        main_context().log().heartbeat();
        _should_log_heartbeat.reset();
    }
}
//------------------------------------------------------------------------------
void process_watchdog::announce_shutdown() noexcept {
    _backend->announce_shutdown();
}
//------------------------------------------------------------------------------
[[nodiscard]] auto process_watchdog::start_watch() noexcept
  -> watched_process_lifetime {
    return {*this};
}
//------------------------------------------------------------------------------
// watched_process_lifetime
//------------------------------------------------------------------------------
watched_process_lifetime::watched_process_lifetime(process_watchdog& wd) noexcept
  : _wd{&wd} {
    _wd->declare_initialized();
}
//------------------------------------------------------------------------------
watched_process_lifetime::~watched_process_lifetime() noexcept {
    if(_wd) {
        _wd->announce_shutdown();
    }
}
//------------------------------------------------------------------------------
void watched_process_lifetime::notify() noexcept {
    assert(_wd);
    _wd->notify_alive();
}
//------------------------------------------------------------------------------
} // namespace eagine
