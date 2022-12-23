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
//------------------------------------------------------------------------------
void fixed_size_string_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    using namespace eagine;

    memory::fixed_size_string<1> f;
    test.check(f.empty(), "empty");
    test.check_equal(f.size(), 0U, "size is zero f");
    test.check(f.begin() == f.end(), "begin == end f");
    test.ensure(f.c_str() != nullptr, "non-null ptr");
    test.check(f.c_str()[0] == '\0', "zero terminated");

    eagine::string_view v = f;
    test.check_equal(v.size(), 0, "size is zero v");
    test.check(v.begin() == v.end(), "begin == end v");
}
//------------------------------------------------------------------------------
void fixed_size_string_content(auto& s) {
    eagitest::case_ test{s, 2, "content"};
    using namespace eagine;

    memory::fixed_size_string<4> f('a', 'b', 'c', '\0');
    test.check(!f.empty(), "not empty");
    test.check_equal(f.size(), 3U, "size v");
    test.check(f.begin() != f.end(), "begin != end f");
    test.ensure(f.c_str() != nullptr, "non-null ptr");
    test.check(std::strcmp(f.c_str(), "abc") == 0, "compare");
    test.check(f.c_str()[f.size()] == '\0', "zero terminated");

    eagine::string_view v = f;
    test.check_equal(v.size(), span_size(std::strlen("abc")), "size v");
    test.check(v.begin() != v.end(), "begin != end v");
}
//------------------------------------------------------------------------------
void fixed_size_string_literal(auto& s) {
    eagitest::case_ test{s, 3, "literal"};
    using namespace eagine;
    const char l[] = "literal";

    auto f = memory::make_fixed_size_string(l);
    test.check(!f.empty(), "not empty");
    test.check_equal(f.size(), span_size(std::strlen(l)), "size v");
    test.check(f.begin() != f.end(), "begin != end f");
    test.ensure(f.c_str() != nullptr, "non-null ptr");
    test.check(std::strcmp(f.c_str(), "literal") == 0, "compare");
    test.check(f.c_str()[f.size()] == '\0', "zero terminated");

    eagine::string_view v = f;
    test.check_equal(v.size(), span_size(7), "size v");
    test.check(v.begin() != v.end(), "begin != end v");
}
//------------------------------------------------------------------------------
void fixed_size_string_concat(auto& s) {
    eagitest::case_ test{s, 4, "concat"};
    using namespace eagine;
    using eagine::memory::make_fixed_size_string;

    const char one[] = "one";
    const char two[] = "two";
    const char three[] = "three";
    const char four[] = "four";

    auto f = make_fixed_size_string(one) + make_fixed_size_string(two) +
             make_fixed_size_string(three) + make_fixed_size_string(four);

    test.check(!f.empty(), "not empty");
    test.ensure(std::strcmp(f.c_str(), "onetwothreefour") == 0, "content 1");
    test.check(f.c_str()[f.size()] == '\0', "zero terminated");

    eagine::string_view v = f;
    test.check_equal(
      v.size(), span_size(std::strlen("onetwothreefour")), "content 2");
    test.check(v.begin() != v.end(), "begin != end");
}
//------------------------------------------------------------------------------
void fixed_size_string_from_int(auto& s) {
    eagitest::case_ test{s, 5, "from int"};
    using namespace eagine;
    using eagine::memory::to_fixed_size_string;

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<0>()).c_str(), "0") == 0,
      "0");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<1>()).c_str(), "1") == 0,
      "1");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<-1>()).c_str(), "-1") == 0,
      "-1");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<9>()).c_str(), "9") == 0,
      "9");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<-9>()).c_str(), "-9") == 0,
      "-9");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<10>()).c_str(), "10") == 0,
      "10");

    test.check(
      std::strcmp(to_fixed_size_string(int_constant<-10>()).c_str(), "-10") ==
        0,
      "-10");

    test.check(
      std::strcmp(
        to_fixed_size_string(int_constant<1234567890>()).c_str(),
        "1234567890") == 0,
      "1234567890");

    test.check(
      std::strcmp(
        to_fixed_size_string(int_constant<-1234567890>()).c_str(),
        "-1234567890") == 0,
      "-1234567890");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "fixed_size_string", 3};
    test.once(fixed_size_string_default_construct);
    test.once(fixed_size_string_content);
    test.once(fixed_size_string_literal);
    // TODO
    // test.once(fixed_size_string_concat);
    // test.once(fixed_size_string_from_int);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
