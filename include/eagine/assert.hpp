/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_ASSERT_HPP
#define EAGINE_ASSERT_HPP

#include "branch_predict.hpp"
#include "config/basic.hpp"
#include "diagnostic.hpp"
#include "maybe_unused.hpp"
#include "stacktrace.hpp"
#include <cassert>

#ifndef EAGINE_CHECK_LIMIT_CAST
#if EAGINE_LOW_PROFILE
#define EAGINE_CHECK_LIMIT_CAST 0
#else
#define EAGINE_CHECK_LIMIT_CAST 1
#endif
#endif

#if EAGINE_USE_STACKTRACE

#define BOOST_ENABLE_ASSERT_DEBUG_HANDLER 1

#include <boost/assert.hpp>

// clang-format off

namespace boost {
//------------------------------------------------------------------------------
inline void assertion_failed(
    const char* expression,
    const char* function,
    const char* filepath,
    const long linenumber) {
    std::cerr
        << filepath << ':' << linenumber
        << ": assertion '"
        << expression
        << "' failed in function `"
        << function
        << "`\n"
        << "Backtrace:\n"
        << eagine::stacktrace
        << std::endl;
    std::abort();
}
//------------------------------------------------------------------------------
inline void assertion_failed_msg(
    const char* expression,
    const char* message,
    const char* function,
    const char* filepath,
    long linenumber) {
    std::cerr
        << filepath << ':' << linenumber
        << ": assertion '"
        << expression
        << "' failed in function `"
        << function
        << "`: "
        << (message ? message : "<...>")
        << '\n'
        << "Backtrace:\n"
        << eagine::stacktrace
        << std::endl;
    std::abort();
}
//------------------------------------------------------------------------------
} // namespace boost

#define EAGINE_ASSERT(EXPR) \
    BOOST_ASSERT(EXPR) // NOLINT(hicpp-no-array-decay)
// clang-format on

#ifdef __clang__
EAGINE_DIAG_POP()
#endif

#else // EAGINE_USE_STACKTRACE

// clang-format off
#define EAGINE_ASSERT(EXPR) \
    assert(EXPR) // NOLINT(cert-dcl03-c,hicpp-static-assert,hicpp-no-array-decay)
// clang-format on

#endif

#if EAGINE_CHECK_LIMIT_CAST
#define EAGINE_CONSTEXPR_ASSERT(UNUSED, RESULT) \
    (EAGINE_MAYBE_UNUSED(UNUSED), RESULT)
#else
#define EAGINE_CONSTEXPR_ASSERT(CHECK, RESULT) \
    ((EAGINE_LIKELY(CHECK) ? void(0)                                  \
                           : [] { EAGINE_ASSERT(!bool(#CHECK)); }()), \
     RESULT)
#endif

#define EAGINE_ABORT(MSG) EAGINE_ASSERT(!bool(MSG))
#define EAGINE_UNREACHABLE(MSG) EAGINE_ABORT(MSG)

#endif // EAGINE_ASSERT_HPP
