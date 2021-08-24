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
} // namespace eagine

