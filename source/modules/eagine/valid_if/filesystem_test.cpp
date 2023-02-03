/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.valid_if;
import <filesystem>;
import <string>;
import <string_view>;
//------------------------------------------------------------------------------
void valid_if_default_construct_file(auto& s) {
    eagitest::case_ test{s, 1, "default construct file"};

    eagine::valid_if_existing_file<std::string> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](const auto&) { return true; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
void valid_if_default_construct_dir(auto& s) {
    eagitest::case_ test{s, 2, "default construct directory"};

    eagine::valid_if_existing_directory<std::string_view> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](auto) { return true; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "valid_if filesystem", 2};
    test.once(valid_if_default_construct_file);
    test.once(valid_if_default_construct_dir);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
