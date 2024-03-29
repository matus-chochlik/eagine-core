/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.progress;

import std;
import eagine.core.types;
import eagine.core.utility;
import eagine.core.logging;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
auto root_activity_init_backend(logger& parent)
  -> unique_holder<progress_tracker_backend> {
    return make_default_progress_tracker_backend(parent);
}
//------------------------------------------------------------------------------
root_activity::root_activity(logger& parent) noexcept
  : activity_progress{root_activity_init_backend(parent)} {}
//------------------------------------------------------------------------------
auto root_activity::register_observer(progress_observer& observer) noexcept
  -> bool {
    if(auto pbe{backend()}) {
        return pbe->register_observer(observer);
    }
    return false;
}
//------------------------------------------------------------------------------
void root_activity::unregister_observer(progress_observer& observer) noexcept {
    if(auto pbe{backend()}) {
        pbe->unregister_observer(observer);
    }
}
//------------------------------------------------------------------------------
void root_activity::set_update_callback(
  const callable_ref<bool() noexcept> callback,
  const std::chrono::milliseconds min_interval) noexcept {
    if(auto pbe{backend()}) {
        pbe->set_update_callback(callback, min_interval);
    }
}
//------------------------------------------------------------------------------
void root_activity::reset_update_callback() noexcept {
    if(auto pbe{backend()}) {
        pbe->reset_update_callback();
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
