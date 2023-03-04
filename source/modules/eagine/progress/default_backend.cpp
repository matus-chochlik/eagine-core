/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.progress:default_backend;

import eagine.core.logging;
import :backend;
import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Creates a default implementation of activity progress backend object.
/// @ingroup progress
export [[nodiscard]] auto make_default_progress_tracker_backend(logger& parent)
  -> std::unique_ptr<progress_tracker_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
