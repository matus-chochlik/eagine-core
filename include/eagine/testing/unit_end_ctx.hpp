/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
///
#include "unit_end.hpp"
import eagine.core.main_ctx;

namespace eagitest {
//------------------------------------------------------------------------------
auto ctx_suite::once(void (*func)(eagitest::ctx_suite&)) noexcept
  -> ctx_suite& {
    try {
        func(*this);
    } catch(const abort_test_case&) {
    } catch(...) {
    }
    return *this;
}
//------------------------------------------------------------------------------
auto ctx_suite::repeat(
  unsigned count,
  void (*func)(unsigned, eagitest::ctx_suite&)) noexcept -> ctx_suite& {
    for(unsigned i = 0; i < count; ++i) {
        try {
            func(i, *this);
        } catch(const abort_test_case&) {
        } catch(...) {
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagitest
