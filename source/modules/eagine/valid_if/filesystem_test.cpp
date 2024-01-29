/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.valid_if;
//------------------------------------------------------------------------------
void valid_if_default_construct_file(auto& s) {
    eagitest::case_ test{s, 1, "default construct file"};

    eagine::valid_if_existing_file<std::string> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.transform([](const auto&) { return true; }).has_value(),
      "and then");
}
//------------------------------------------------------------------------------
void valid_if_default_construct_dir(auto& s) {
    eagitest::case_ test{s, 2, "default construct directory"};

    eagine::valid_if_existing_directory<std::filesystem::path> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.transform([](auto) { return true; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
void valid_if_executable_path(auto& s) {
    eagitest::case_ test{s, 3, "executable path"};

    eagine::valid_if_existing_file<std::string_view> v{test.executable_path()};

    test.check(v.has_value(), "has value");
    test.check(not not v, "is not false");
    test.check(bool(v), "is true");

    test.check(
      v.transform([](std::string_view p) { return p.size(); }).value_or(0) > 0,
      "length > 0");
    test.check(
      v.transform([](std::string_view p) -> bool {
           return p.find("filesystem") != std::string_view::npos;
       })
        .or_false(),
      "content");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "valid_if filesystem", 3};
    test.once(valid_if_default_construct_file);
    test.once(valid_if_default_construct_dir);
    test.once(valid_if_executable_path);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
