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
void user_info_config_dir_path(auto& s) {
    eagitest::case_ test{s, 1, "config dir path"};

    eagine::user_info ui;

    test.check(ui.config_dir_path().has_value(), "path ok");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "user info", 1};
    test.once(user_info_config_dir_path);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
