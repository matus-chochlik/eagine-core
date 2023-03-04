/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import <csignal>;

namespace eagine {
//------------------------------------------------------------------------------
static auto signal_switch_state() noexcept -> volatile std::sig_atomic_t& {
    static volatile std::sig_atomic_t state{0};
    return state;
}
//------------------------------------------------------------------------------
signal_switch::signal_switch() noexcept
  : _intr_handler{std::signal(SIGINT, &_flip)}
  , _term_handler{std::signal(SIGTERM, &_flip)} {}
//------------------------------------------------------------------------------
signal_switch::~signal_switch() noexcept {
    [[maybe_unused]] const auto unused_int{std::signal(SIGINT, _intr_handler)};
    [[maybe_unused]] const auto unused_trm{std::signal(SIGTERM, _term_handler)};
}
//------------------------------------------------------------------------------
auto signal_switch::reset() noexcept -> signal_switch& {
    signal_switch_state() = 0;
    return *this;
}
//------------------------------------------------------------------------------
auto signal_switch::interrupted() const noexcept -> bool {
    return signal_switch_state() == SIGINT;
}
//------------------------------------------------------------------------------
auto signal_switch::terminated() const noexcept -> bool {
    return signal_switch_state() == SIGTERM;
}
//------------------------------------------------------------------------------
signal_switch::operator bool() const noexcept {
    return bool(signal_switch_state());
}
//------------------------------------------------------------------------------
void signal_switch::_flip(const int sig_num) noexcept {
    signal_switch_state() = sig_num;
}
//------------------------------------------------------------------------------
} // namespace eagine
