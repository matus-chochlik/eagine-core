/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.progress:default_backend;

import :backend;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
export struct main_ctx_getters;

/// @brief Creates a default implementation of activity progress backend object.
/// @ingroup progress
export auto make_default_progress_tracker_backend(main_ctx_getters&)
  -> std::unique_ptr<progress_tracker_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
