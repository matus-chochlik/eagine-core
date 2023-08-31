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
auto scheduler::schedule_repeated(
  const identifier id,
  const duration_type interval,
  std::function<void()> action) -> scheduler& {
    _repeated[id] = {
      .next = _clock.now() + interval,
      .interval = interval,
      .action = std::move(action)};
    return *this;
}
//------------------------------------------------------------------------------
auto scheduler::remove(const identifier id) -> scheduler& {
    _repeated.erase(id);
    return *this;
}
//------------------------------------------------------------------------------
auto scheduler::update() noexcept -> scheduler& {
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

