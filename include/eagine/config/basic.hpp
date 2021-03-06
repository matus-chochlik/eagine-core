/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_CONFIG_BASIC_HPP
#define EAGINE_CONFIG_BASIC_HPP

#if __has_include(<eagine/config/site.hpp>)
#include <eagine/config/site.hpp>
#endif

#ifndef EAGINE_LOW_PROFILE
/// @brief Enables or disables "low-profile" mode.
/// @ingroup build_config
///
/// In low-profile mode some of the expensive checks and features, used mostly
/// for debugging and development, are disabled. This typically results in
/// better run-time performance and may shorten the build times at the expense
/// of losing some information useful for checking the code.
#define EAGINE_LOW_PROFILE 0
#endif

#ifndef EAGINE_DEBUG
/// @brief Enables or disables debugging helper utilities.
/// @ingroup build_config
#define EAGINE_DEBUG 1
#endif

#ifndef EAGINE_USE_ZLIB
#define EAGINE_USE_ZLIB 0
#endif

#ifndef EAGINE_USE_BACKTRACE
#define EAGINE_USE_BACKTRACE 0
#endif

#ifndef EAGINE_USE_STACKTRACE
#define EAGINE_USE_STACKTRACE 0
#endif

#ifndef EAGINE_USE_BOOST_SPIRIT
#define EAGINE_USE_BOOST_SPIRIT 1
#endif

#ifndef EAGINE_USE_SYSTEMD
#define EAGINE_USE_SYSTEMD 0
#endif

#ifndef EAGINE_CORE_LIBRARY
#if defined(__clang_analyzer__)
#define EAGINE_CORE_LIBRARY 1
#else
#define EAGINE_CORE_LIBRARY 0
#endif
#endif

#ifndef EAGINE_LIB_FUNC
#define EAGINE_LIB_FUNC inline
#endif

#ifndef EAGINE_CXX_REFLECTION
#define EAGINE_CXX_REFLECTION 0
#endif

#ifndef EAGINE_CXX_CONSTEVAL
#if EAGINE_CXX_REFLECTION || \
  (defined(__cpp_consteval) && __cpp_consteval >= 201811L)
#define EAGINE_CXX_CONSTEVAL 1
#else
#define EAGINE_CXX_CONSTEVAL 0
#endif
#endif

#endif // EAGINE_CONFIG_BASIC_HPP
