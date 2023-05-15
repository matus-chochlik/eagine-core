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

private:
    std::shared_ptr<process_watchdog_backend> _backend;
    timeout _should_log_heartbeat;
};

} // namespace eagine

