/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_ROOT_ACTIVITY_HPP
#define EAGINE_PROGRESS_ROOT_ACTIVITY_HPP

#include "../program_args.hpp"
#include "activity.hpp"

namespace eagine {
struct main_ctx_getters;
//------------------------------------------------------------------------------
class root_activity : public activity_progress {
public:
    root_activity(main_ctx_getters& ctx) noexcept
      : activity_progress{_init_backend(ctx)} {}

    void set_update_callback(
      const callable_ref<bool()> callback,
      const std::chrono::milliseconds min_interval) {
        if(auto pbe{backend()}) {
            extract(pbe).set_update_callback(callback, min_interval);
        }
    }

private:
    static auto _init_backend(main_ctx_getters&)
      -> std::unique_ptr<progress_tracker_backend>;
};
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/progress/root_activity.inl>
#endif

#endif // EAGINE_PROGRESS_ROOT_ACTIVITY_HPP
