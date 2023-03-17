/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.main_ctx;
//------------------------------------------------------------------------------
void system_info_uptime(auto& s) {
    eagitest::case_ test{s, 1, "uptime"};

    eagine::system_info si;

    test.check(si.uptime().count() > 0.F, "uptime ok");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "system info", 1};
    test.once(system_info_uptime);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
