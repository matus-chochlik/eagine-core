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
  std::function<void()> action) -> action_scheduler& {
    _repeated[id] = {
      .next = _clock.now() + interval,
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
auto action_scheduler::update() noexcept -> action_scheduler& {
    const duration_type zero{0};
    for(auto& entry : _repeated.underlying()) {
        const auto now{_clock.now()};
        auto& scheduled{std::get<1>(entry)};
        auto overdue{now - scheduled.next};
        while(overdue >= zero) {
            scheduled.action();
            scheduled.next = now + scheduled.interval - overdue;
            overdue -= scheduled.interval;
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagine

