/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_CONFIG_PLATFORM_HPP
#define EAGINE_CONFIG_PLATFORM_HPP

#ifndef EAGINE_POSIX
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define EAGINE_POSIX 1
#else
#define EAGINE_POSIX 0
#endif
#endif

#ifndef EAGINE_LINUX
#if defined(__linux__)
#define EAGINE_LINUX 1
#else
#define EAGINE_LINUX 0
#endif
#endif

#ifndef EAGINE_APPLE
#if defined(__APPLE__) && __APPLE__
#define EAGINE_APPLE 1
#else
#define EAGINE_APPLE 0
#endif
#endif

#ifndef EAGINE_WINDOWS
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define EAGINE_WINDOWS 1
#else
#define EAGINE_WINDOWS 0
#endif
#endif

#endif // EAGINE_CONFIG_PLATFORM_HPP
