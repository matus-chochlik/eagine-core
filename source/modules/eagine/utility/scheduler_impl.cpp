/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.utility;

import std;
import eagine.core.types;
import eagine.core.identifier;

namespace eagine {
//------------------------------------------------------------------------------
auto action_scheduler::schedule_repeated(
  const identifier id,
  const duration_type interval,
  std::function<bool()> action) -> action_scheduler& {
    _repeated[id] = {
      .next = now() + interval,
      .interval = interval,
      .action = std::move(action)};
    return *this;
}
//------------------------------------------------------------------------------
auto action_scheduler::remove(const identifier id) -> action_scheduler& {
    _repeated.erase(id);
    return *this;
}
//------------------------------------------------------------------------------
auto action_scheduler::_scheduled::should_invoke() noexcept -> bool {
    if(fail_counter != 0U) [[unlikely]] {
        --fail_counter;
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
void action_scheduler::_scheduled::invoke() noexcept {
    if(action()) [[likely]] {
        fail_counter = 0U;
    } else {
        ++fail_counter;
    }
}
//------------------------------------------------------------------------------
void action_scheduler::_scheduled::update(
  const std::chrono::steady_clock::time_point now) noexcept {
    const action_scheduler::duration_type zero{0};
    auto overdue{now - next};
    while(overdue >= zero) {
        if(should_invoke()) {
            invoke();
        }
        next = now + interval - overdue;
        overdue -= interval;
    }
}
//------------------------------------------------------------------------------
auto action_scheduler::update() noexcept -> action_scheduler& {
    for(auto& entry : _repeated.underlying()) {
        std::get<1>(entry).update(now());
    }
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagine

