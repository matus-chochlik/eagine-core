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
import eagine.core.runtime;
//------------------------------------------------------------------------------
void basic_config_find_1(auto& s) {
    eagitest::case_ test{s, 1, "find 1"};
    std::array<const char*, 2> argv{{"basic_config_test", nullptr}};
    eagine::program_args args{1, argv.data()};

    eagine::basic_config cfg{args};

    test.check_equal(cfg.find("var.module-1").value_or("foo"), "eagine", "A");
    test.check_equal(cfg.find("var.module.2").value_or("bar"), "core", "B");
    test.check_equal(cfg.find("var-module-3").value_or("baz"), "runtime", "C");
    test.check_equal(cfg.find("var-module-4").value_or("qux"), "qux", "D");
    test.check_equal(cfg.find("var_flag_true").value_or("none"), "true", "E");
    test.check_equal(cfg.find("var_flag_false").value_or("none"), "false", "F");
    test.check_equal(cfg.find("var-12345").value_or("none"), "12345", "G");
    test.check_equal(cfg.find("var-none").value_or("none"), "none", "H");
    test.check(not cfg.find("var-none").has_value(), "I");
}
//------------------------------------------------------------------------------
void basic_config_find_2(auto& s) {
    eagitest::case_ test{s, 2, "find 2"};
    std::array<const char*, 10> argv{
      {"basic_config_test",
       "--var-module-3",
       "Runtime",
       "--var-module-2",
       "Core",
       "--var-module-1",
       "EAGine",
       "--var-12345",
       "23456",
       nullptr}};
    eagine::program_args args{9, argv.data()};

    eagine::basic_config cfg{args};

    test.check_equal(cfg.find("var.module-1").value_or("foo"), "EAGine", "A");
    test.check_equal(cfg.find("var.module.2").value_or("bar"), "Core", "B");
    test.check_equal(cfg.find("var-module-3").value_or("baz"), "Runtime", "C");
    test.check_equal(cfg.find("var-module-4").value_or("qux"), "qux", "D");
    test.check_equal(cfg.find("var-flag_true").value_or("none"), "true", "E");
    test.check_equal(cfg.find("var-flag_false").value_or("none"), "false", "F");
    test.check_equal(cfg.find("var-12345").value_or("none"), "23456", "G");
    test.check_equal(cfg.find("var-none").value_or("none"), "none", "H");
    test.check(not cfg.find("var-none").has_value(), "I");
}
//------------------------------------------------------------------------------
void basic_config_is_set_1(auto& s) {
    eagitest::case_ test{s, 3, "is-set 1"};
    std::array<const char*, 2> argv{{"basic_config_test", nullptr}};
    eagine::program_args args{1, argv.data()};

    eagine::basic_config cfg{args};

    test.check(cfg.is_set("var_flag-true"), "A");
    test.check(not cfg.is_set("var_flag-false"), "B");
    test.check(cfg.is_set("var_flag"), "C");
    test.check(cfg.is_set("var-12345"), "D");
}
//------------------------------------------------------------------------------
void basic_config_is_set_2(auto& s) {
    eagitest::case_ test{s, 4, "is-set 2"};
    std::array<const char*, 9> argv{
      {"basic_config_test",
       "--var-flag-12345",
       "false",
       "--arg-flag",
       "--arg-flag-true",
       "true",
       "--arg-flag-false",
       "false",
       nullptr}};
    eagine::program_args args{1, argv.data()};

    eagine::basic_config cfg{args};

    test.check(cfg.is_set("arg.flag-true"), "A");
    test.check(not cfg.is_set("arg-flag_false"), "B");
    test.check(cfg.is_set("arg.flag"), "C");
    test.check(not cfg.is_set("var-12345"), "D");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "basic_config", 4};
    test.once(basic_config_find_1);
    test.once(basic_config_find_2);
    test.once(basic_config_is_set_1);
    test.once(basic_config_is_set_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
