/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/main_ctx_fwd.hpp>
#include <eagine/progress/default_backend.hpp>

namespace eagine {
//------------------------------------------------------------------------------
auto root_activity::_init_backend(main_ctx_getters& ctx)
  -> std::unique_ptr<progress_tracker_backend> {
    return std::make_unique<default_progress_tracker_backend>(ctx);
}
//------------------------------------------------------------------------------
auto root_activity::register_observer(progress_observer& observer) noexcept
  -> bool {
    if(auto pbe{backend()}) {
        return extract(pbe).register_observer(observer);
    }
    return false;
}
//------------------------------------------------------------------------------
void root_activity::unregister_observer(progress_observer& observer) noexcept {
    if(auto pbe{backend()}) {
        extract(pbe).unregister_observer(observer);
    }
}
//------------------------------------------------------------------------------
void root_activity::set_update_callback(
  const callable_ref<bool() noexcept> callback,
  const std::chrono::milliseconds min_interval) noexcept {
    if(auto pbe{backend()}) {
        extract(pbe).set_update_callback(callback, min_interval);
    }
}
//------------------------------------------------------------------------------
void root_activity::reset_update_callback() noexcept {
    if(auto pbe{backend()}) {
        extract(pbe).reset_update_callback();
    }
}
//------------------------------------------------------------------------------
} // namespace eagine

