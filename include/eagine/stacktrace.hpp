/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_STACKTRACE_HPP
#define EAGINE_STACKTRACE_HPP

#include "config/basic.hpp"
#include "diagnostic.hpp"
#include <iostream>

namespace eagine {

struct stacktrace_t {};
constinit inline stacktrace_t stacktrace{};

} // namespace eagine

#if EAGINE_USE_STACKTRACE

#ifdef __clang__
EAGINE_DIAG_PUSH()
EAGINE_DIAG_OFF(shadow)
EAGINE_DIAG_OFF(missing-noreturn)
#endif

#if EAGINE_USE_BACKTRACE
#define BOOST_STACKTRACE_USE_BACKTRACE 1
#endif

#include <boost/stacktrace/stacktrace.hpp>

namespace eagine {

static inline auto operator<<(std::ostream& out, const stacktrace_t)
  -> std::ostream& {
    return out << ::boost::stacktrace::stacktrace();
}

} // namespace eagine

#else
namespace eagine {

static inline auto operator<<(std::ostream& out, const stacktrace_t)
  -> std::ostream& {
    return out << "[N/A]";
}

} // namespace eagine

#endif

#endif
