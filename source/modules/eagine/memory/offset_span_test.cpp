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
import <cstring>;
//------------------------------------------------------------------------------
void offset_span_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    using namespace eagine;
    memory::offset_span<double> a;

    test.check(a.data() == nullptr, "is nullptr");
    test.check(a.addr() == memory::const_address(), "has default address");
    test.check(a.size() == 0, "size is zero");
    test.check(a.begin() == a.end(), "begin == end");
}
//------------------------------------------------------------------------------
void offset_span_construct_1(auto& s) {
    eagitest::case_ test{s, 2, "construct 1"};
    eagitest::track trck{test, 20, 2};
    using namespace eagine;

    int ia[10] = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};

    memory::offset_span<int> a(ia, 10);

    test.check(a.data() == ia, "data");
    test.check(a.addr() == memory::const_address(ia), "addr");
    test.check(a.size() == 10, "size");
    test.check(a.begin() != a.end(), "begin != end");

    int i = 0;
    for(int e : a) {
        test.check_equal(e, i * i, "range element");
        ++i;
        trck.passed_part(1);
    }

    for(i = 0; i < 10; ++i) {
        test.check_equal(a[eagine::span_size(i)], i * i, "element at index");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
void offset_span_construct_2(auto& s) {
    eagitest::case_ test{s, 3, "construct 2"};
    eagitest::track trck{test, 26, 2};
    using namespace eagine;

    const char* cstr = "FooBarBazBlah";

    memory::offset_span<const char> acc(cstr, span_size(std::strlen(cstr)));

    test.check(acc.data() == cstr, "data");
    test.check(acc.addr() == memory::const_address(cstr), "addr");
    test.ensure(acc.size() == span_size(std::strlen(cstr)), "size");
    test.check(acc.begin() != acc.end(), "begin != end");

    span_size_t i = 0;
    for(int e : acc) {
        test.check_equal(e, cstr[i], "range element");
        ++i;
        trck.passed_part(1);
    }

    for(i = 0; i < acc.size(); ++i) {
        test.check_equal(acc[i], cstr[i], "element at index");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
void offset_span_construct_3(auto& s) {
    eagitest::case_ test{s, 4, "construct 3"};
    eagitest::track trck{test, 26, 2};
    using namespace eagine;

    string_view cstr = "FooBarBazBlah";

    memory::offset_span<const char> acc(cstr);

    test.check(acc.data() == cstr.data(), "data");
    test.check(acc.addr() == cstr.addr(), "addr");
    test.ensure(acc.size() == cstr.size(), "size");
    test.check(acc.begin() != acc.end(), "begin != end");

    span_size_t i = 0;
    for(int e : acc) {
        test.check_equal(e, cstr[i], "range element");
        ++i;
        trck.passed_part(1);
    }

    for(i = 0; i < acc.size(); ++i) {
        test.check_equal(acc[i], cstr[i], "element at index");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
void offset_span_construct_4(auto& s) {
    eagitest::case_ test{s, 5, "construct 4"};
    eagitest::track trck{test, 40, 2};
    using namespace eagine;
    int ia[20] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

    const memory::offset_span<int> cai(ia, 20);

    test.check(cai.data() == ia, "data");
    test.check(cai.addr() == memory::const_address(ia), "addr");
    test.check(cai.size() == 20, "size");
    test.check(cai.begin() != cai.end(), "begin != end");

    memory::offset_span<int>::size_type i = 0;
    for(const int e : cai) {
        test.check_equal(e, ++i, "range element");
        trck.passed_part(1);
    }

    for(i = 0; i < cai.size(); ++i) {
        test.check_equal(cai[i], ia[i], "element at index");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "offset_span", 5};
    test.once(offset_span_default_construct);
    test.once(offset_span_construct_1);
    test.once(offset_span_construct_2);
    test.once(offset_span_construct_3);
    test.once(offset_span_construct_4);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
