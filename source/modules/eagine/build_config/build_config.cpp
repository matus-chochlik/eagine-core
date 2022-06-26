/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_config;

namespace eagine {

export constexpr const bool debug_build{EAGINE_DEBUG != 0};
export constexpr const bool low_profile_build{EAGINE_LOW_PROFILE != 0};

} // namespace eagine
