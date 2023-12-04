/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:watchdog;

import std;
import eagine.core.types;
import eagine.core.utility;
import eagine.core.identifier;
import :interface;
import :parent;
import :object;

namespace eagine {

struct process_watchdog_backend;
export class watched_process_lifetime;
//------------------------------------------------------------------------------
/// @brief Class implementing process watchdog functionality.
/// @ingroup main_context
export class process_watchdog : public main_ctx_object {
public:
    process_watchdog(main_ctx_parent parent) noexcept;

    /// @brief Tells the system that this process finished initialization.
    void declare_initialized() noexcept;

    /// @brief Tells the system that this process is alive. Should be called repeatedly.
    void notify_alive() noexcept;

    /// @brief Announce to the system that this process is shutting down.
    void announce_shutdown() noexcept;

    [[nodiscard]] auto start_watch() noexcept -> watched_process_lifetime;

private:
    shared_holder<process_watchdog_backend> _backend;
    timeout _should_log_heartbeat;
};
//------------------------------------------------------------------------------
export class watched_process_lifetime {
public:
    watched_process_lifetime(process_watchdog& wd) noexcept;

    watched_process_lifetime(watched_process_lifetime&& that) noexcept
      : _wd{std::exchange(that._wd, nullptr)} {}

    watched_process_lifetime(const watched_process_lifetime&) = delete;

    auto operator=(watched_process_lifetime&&)
      -> watched_process_lifetime& = delete;
    auto operator=(const watched_process_lifetime&)
      -> watched_process_lifetime& = delete;

    ~watched_process_lifetime() noexcept;

    constexpr explicit operator bool() noexcept {
        return true;
    }

    void notify() noexcept;

private:
    process_watchdog* _wd{nullptr};
};
//------------------------------------------------------------------------------
} // namespace eagine

