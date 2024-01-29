/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:signal_switch;

import std;
import eagine.core.utility;

namespace eagine {

/// @brief Installs handlers for interrupt signals and flips switch on receipt.
///
/// This class installs handlers for the interrupt and terminate OS IPC signals
/// and if one of the signals is received it flips a boolean indicator from
/// false to true.
///
/// @note Only a single instance per process should be created.
export class signal_switch {
public:
    /// @brief Default constructor. Stores the original handlers if any.
    /// @post not bool(*this)
    signal_switch() noexcept;

    /// @brief Destructor. Restores the original signal handlers.
    ~signal_switch() noexcept;

    /// @brief Not moveable.
    signal_switch(signal_switch&&) = delete;
    /// @brief Not copyable.
    signal_switch(const signal_switch&) = delete;
    /// @brief Not move-assignable.
    auto operator=(signal_switch&&) = delete;
    /// @brief Not copy-assignable.
    auto operator=(const signal_switch&) = delete;

    /// @brief Resets the signal state as if no signal was received.
    /// @post not bool(*this)
    auto reset() noexcept -> signal_switch&;

    /// @brief Indicates if the interrupt signal was received.
    auto interrupted() const noexcept -> bool;

    /// @brief Indicates if the terminate signal was received.
    auto terminated() const noexcept -> bool;

    /// @brief Indicates if one of the tracked signals was received.
    explicit operator bool() const noexcept;

    /// @brief Invoked when the state of this switch changes.
    signal<void(bool) noexcept> switched;

private:
    static auto _state() noexcept -> volatile std::sig_atomic_t&;

    static void _flip(const int sig_num) noexcept;

    using _sighandler_t = void(int);
    _sighandler_t* _intr_handler{nullptr};
    _sighandler_t* _term_handler{nullptr};
};

} // namespace eagine
