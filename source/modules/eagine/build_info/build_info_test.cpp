/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.build_info;
//------------------------------------------------------------------------------
void compiler_info_data(auto& s) {
    eagitest::case_ test{s, 1, "compiler info"};

    eagine::compiler_info i;
    test.check(i.name().has_value(), "has compiler name");
}
//------------------------------------------------------------------------------
void version_info_data(auto& s) {
    eagitest::case_ test{s, 2, "version info"};

    eagine::version_info i;
    test.check(i.git().has_value(), "has git version");
    test.check(
      not not i.git().version_at_least(0, 0, 0, 1), "git version at least");
}
//------------------------------------------------------------------------------
void build_info_data(auto& s) {
    eagitest::case_ test{s, 3, "paths"};

    eagine::build_info i;
    test.check(i.install_prefix().has_value(), "install prefix");
    test.check(i.config_dir_path().has_value(), "config dir path");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "build info", 3};
    test.once(compiler_info_data);
    test.once(version_info_data);
    test.once(build_info_data);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
