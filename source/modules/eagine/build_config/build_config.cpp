/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<eagine/config/site.hpp>)
#include <eagine/config/site.hpp>
#endif

export module eagine.core.build_config;

namespace eagine {

#ifdef EAGINE_DEBUG
export constexpr const bool debug_build{EAGINE_DEBUG != 0};
#else
export constexpr const bool debug_build{false};
#endif

#ifdef EAGINE_LOW_PROFILE
export constexpr const bool low_profile_build{EAGINE_LOW_PROFILE != 0};
#else
export constexpr const bool low_profile_build{false};
#endif

} // namespace eagine
