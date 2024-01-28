/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin_ctx.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.main_ctx;
//------------------------------------------------------------------------------
void system_info_uptime(auto& s) {
    eagitest::case_ test{s, 1, "uptime"};

    eagine::system_info si{s.context()};

    test.check(si.uptime().count() > 0.F, "uptime ok");
}
//------------------------------------------------------------------------------
auto test_main(eagine::test_ctx& ctx) -> int {
    eagitest::ctx_suite test{ctx, "system info", 1};
    test.once(system_info_uptime);
    return test.exit_code();
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    return eagine::test_main_impl(argc, argv, test_main);
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end_ctx.hpp>
