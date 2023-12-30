/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.container;
//------------------------------------------------------------------------------
void chunk_list_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::chunk_list<int, 32> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");
}
//------------------------------------------------------------------------------
void chunk_list_push_back(auto& s) {
    eagitest::case_ test{s, 2, "push back"};
    eagine::chunk_list<int, 31> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
    }
    test.check_equal(i - 1, cl.back(), "back is ok");
}
//------------------------------------------------------------------------------
void chunk_list_emplace_back(auto& s) {
    eagitest::case_ test{s, 3, "emplace back"};
    eagine::chunk_list<int, 27> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.emplace_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
    }
    test.check_equal(i - 1, cl.back(), "back is ok");
}
//------------------------------------------------------------------------------
void chunk_list_insert(auto& s) {
    eagitest::case_ test{s, 4, "insert"};
    eagine::chunk_list<int, 23> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    auto ipos{cl.begin()};

    for(int i = 0; i < 10000; ++i) {
        ipos = cl.insert(ipos, i);
        ++ipos;
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
    }
    test.check_equal(i - 1, cl.back(), "back is ok");

    ipos = cl.begin();
    for(int j = 0; j < 10000; ++j) {
        ipos = cl.insert(ipos, j);
        ipos += 2;
    }

    i = 0;
    for(const auto e : cl) {
        test.check_equal(i++ / 2, e, "element is ok");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "chunk_list", 4};
    test.once(chunk_list_default_construct);
    test.once(chunk_list_push_back);
    test.once(chunk_list_emplace_back);
    test.once(chunk_list_insert);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
