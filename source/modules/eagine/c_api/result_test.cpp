/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.c_api;
//------------------------------------------------------------------------------
void result_1(auto& s) {
    eagitest::case_ test{s, 1, "result 1"};

    eagine::c_api::result<int, eagine::nothing_t> r{123};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");
}
//------------------------------------------------------------------------------
void no_result_1(auto& s) {
    eagitest::case_ test{s, 2, "no result 1"};

    eagine::c_api::no_result<int, eagine::nothing_t> r{};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");
}
//------------------------------------------------------------------------------
void opt_result_1(auto& s) {
    eagitest::case_ test{s, 3, "opt result 1"};

    eagine::c_api::opt_result<int, eagine::nothing_t> r{234, true};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");
}
//------------------------------------------------------------------------------
void combined_result_1(auto& s) {
    eagitest::case_ test{s, 4, "combined result 1"};

    eagine::c_api::opt_result<int, eagine::nothing_t> o{345, true};
    eagine::c_api::combined_result<int, eagine::nothing_t> r{o};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "C-API result", 4};
    test.once(result_1);
    test.once(no_result_1);
    test.once(opt_result_1);
    test.once(combined_result_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
