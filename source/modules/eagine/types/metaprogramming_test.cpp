/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
//------------------------------------------------------------------------------
struct type_A {};
struct type_B {};
struct type_C {};
struct type_D {};
//------------------------------------------------------------------------------
// list
//------------------------------------------------------------------------------
void mp_list_contains(auto& s) {
    eagitest::case_ test{s, 1, "list contains"};

    using namespace eagine;
    using lABC = mp_list<type_A, type_B, type_C>;
    using lDCB = mp_list<type_D, type_C, type_B>;

    test.check(mp_contains_v<lABC, type_A>, "ABC contains A");
    test.check(mp_contains_v<lABC, type_B>, "ABC contains B");
    test.check(mp_contains_v<lABC, type_C>, "ABC contains C");
    test.check(not mp_contains_v<lABC, type_D>, "ABC does not contain D");

    test.check(mp_contains_v<lDCB, type_D>, "DCB contains D");
    test.check(mp_contains_v<lDCB, type_C>, "DCB contains C");
    test.check(mp_contains_v<lDCB, type_B>, "DCB contains B");
    test.check(not mp_contains_v<lDCB, type_A>, "DCB does not contain A");
}
//------------------------------------------------------------------------------
void mp_list_union(auto& s) {
    eagitest::case_ test{s, 2, "list union"};

    using namespace eagine;
    using lABC = mp_list<type_A, type_B, type_C>;
    using lDCB = mp_list<type_D, type_C, type_B>;

    using lBC = mp_union_t<lABC, lDCB>;

    test.check(not mp_contains_v<lBC, type_A>, "does not contain A");
    test.check(mp_contains_v<lBC, type_B>, "contains B");
    test.check(mp_contains_v<lBC, type_C>, "contains C");
    test.check(not mp_contains_v<lBC, type_D>, "does not contain D");
}
//------------------------------------------------------------------------------
// string
//------------------------------------------------------------------------------
void mp_string_empty(auto& s) {
    eagitest::case_ test{s, 3, "string empty"};
    using namespace eagine;

    using str = mp_string<>;
    test.check_equal(std::strlen(str::value), std::size_t(0), "strlen");
    test.check_equal(mp_strlen<str>::value, std::size_t(0), "length");
    test.check(std::strcmp(str::value, "") == 0, "content");
}
//------------------------------------------------------------------------------
void mp_string_content(auto& s) {
    eagitest::case_ test{s, 4, "string content"};
    using namespace eagine;

    using str = mp_string<'m', 'p', '_', 's', 't', 'r', 'i', 'n', 'g'>;
    test.check_equal(
      std::strlen(str::value), std::strlen("mp_string"), "strlen");
    test.check_equal(mp_strlen<str>::value, std::strlen("mp_string"), "length");
    test.check(std::strcmp(str::value, "mp_string") == 0, "content");
}
//------------------------------------------------------------------------------
struct mp_blah {
    static constexpr const char mp_str[] = "blah";
};
//------------------------------------------------------------------------------
void mp_string_make(auto& s) {
    eagitest::case_ test{s, 5, "string make"};
    using namespace eagine;

    using str = mp_make_string_t<mp_blah>;
    test.check_equal(std::strlen(str::value), std::strlen("blah"), "strlen");
    test.check_equal(mp_strlen<str>::value, std::strlen("blah"), "length");
    test.check(std::strcmp(str::value, "blah") == 0, "content");
}
//------------------------------------------------------------------------------
void mp_string_concat(auto& s) {
    eagitest::case_ test{s, 6, "string concatenate"};
    using namespace eagine;

    using foo = mp_string<'f', 'o', 'o'>;
    using bar = mp_string<'b', 'a', 'r'>;
    using baz = mp_string<'b', 'a', 'z'>;

    using foobarbaz = mp_concat_t<foo, bar, baz>;
    using bazbarfoo = mp_concat_t<baz, bar, foo>;

    test.check_equal(
      std::strlen(foobarbaz::value), std::strlen("foobarbaz"), "strlen");
    test.check_equal(
      mp_strlen<bazbarfoo>::value, std::strlen("bazbarfoo"), "length");
    test.check(std::strcmp(foobarbaz::value, "foobarbaz") == 0, "content");
    test.check(std::strcmp(bazbarfoo::value, "bazbarfoo") == 0, "content");
}
//------------------------------------------------------------------------------
template <char C>
struct test_lshift : eagine::mp_string<C - 1> {};
//------------------------------------------------------------------------------
void mp_string_translate(auto& s) {
    eagitest::case_ test{s, 7, "string translate"};
    using namespace eagine;

    using bcdefg = mp_string<'b', 'c', 'd', 'e', 'f', 'g'>;

    using str = mp_translate_t<bcdefg, test_lshift>;

    test.check_equal(std::strlen(str::value), std::strlen("abcdef"), "strlen");
    test.check_equal(mp_strlen<str>::value, std::strlen("abcdef"), "length");
    test.check(std::strcmp(str::value, "abcdef") == 0, "content");
}
//------------------------------------------------------------------------------
void mp_string_int_to_string(auto& s) {
    eagitest::case_ test{s, 8, "string int to string"};
    using namespace eagine;

    using str1 = mp_int_to_string_t<-1234567890>;
    using str2 = mp_int_to_string_t<123456789>;

    test.check_equal(std::strlen(str1::value), std::size_t(11), "strlen");
    test.check_equal(mp_strlen<str1>::value, std::size_t(11), "length");
    test.check(std::strcmp(str1::value, "-1234567890") == 0, "content");
    test.check_equal(std::strlen(str2::value), std::size_t(9), "strlen");
    test.check_equal(mp_strlen<str2>::value, std::size_t(9), "length");
    test.check(std::strcmp(str2::value, "123456789") == 0, "content");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "metaprogramming", 8};
    test.once(mp_list_contains);
    test.once(mp_list_union);
    test.once(mp_string_empty);
    test.once(mp_string_content);
    test.once(mp_string_make);
    test.once(mp_string_concat);
    test.once(mp_string_translate);
    test.once(mp_string_int_to_string);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
