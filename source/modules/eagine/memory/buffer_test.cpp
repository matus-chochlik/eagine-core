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
void buffer_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    using namespace eagine;

    memory::buffer buf;

    test.check(buf.empty(), "is empty");
    test.check_equal(buf.size(), 0, "size is zero");
    test.check_equal(buf.capacity(), 0, "capacity is zero");

    memory::block blk = buf;

    test.check(not bool(blk), "is false");
    test.check(not blk, "is not true");
    test.check(blk.empty(), "is empty");
    test.check_equal(blk.size(), 0, "size is zero");
    test.check(blk.begin() == blk.end(), "begin == end");
}
//------------------------------------------------------------------------------
void buffer_resize(unsigned, auto& s) {
    eagitest::case_ test{s, 2, "resize"};
    eagitest::track trck{test, 0, 2};
    auto& rg{test.random()};
    using namespace eagine;

    memory::buffer buf;

    int r = 0;

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto sz = span_size_t(r * r);

        buf.resize(sz);

        test.check_equal(buf.size(), sz, "size is ok");
        test.check(buf.capacity() >= sz, "capacity is ok");

        memory::block blk = buf;

        if(buf.size() > 0) {
            test.check(bool(blk), "is true");
            test.check(!!blk, "is not false");
            test.check(not blk.empty(), "is not empty");
            test.check_equal(blk.size(), buf.size(), "size is ok");
            test.check(blk.begin() != blk.end(), "begin != end");
            trck.passed_part(1);
        } else {
            test.check(not bool(blk), "is false");
            test.check(not blk, "is not true");
            test.check(blk.empty(), "is empty");
            test.check_equal(blk.size(), 0, "size is 0");
            test.check(blk.begin() == blk.end(), "begin == end");
            trck.passed_part(2);
        }

        r = rg.get_int(0, 1000);
    }
}
//------------------------------------------------------------------------------
void buffer_ensure(unsigned, auto& s) {
    eagitest::case_ test{s, 3, "ensure"};
    eagitest::track trck{test, 0, 2};
    auto& rg{test.random()};
    using namespace eagine;

    memory::buffer buf;

    int r = 0;

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto sz = span_size_t(r * r);

        buf.resize(sz);

        test.check_equal(buf.size(), sz, "size is ok");
        test.check(buf.capacity() >= sz, "capacity is ok");

        memory::block blk = buf;

        if(buf.size() > 0) {
            test.check(bool(blk), "is true");
            test.check(!!blk, "is not false");
            test.check(not blk.empty(), "is not empty");
            test.check(blk.size() >= buf.size(), "size is ok");
            test.check(blk.begin() != blk.end(), "begin != end");
            trck.passed_part(1);
        } else {
            test.check(not bool(blk), "is false");
            test.check(not blk, "is not true");
            test.check(blk.empty(), "is empty");
            test.check(blk.begin() == blk.end(), "begin == end");
            trck.passed_part(2);
        }

        r = rg.get_int(0, 1000);
    }
}
//------------------------------------------------------------------------------
void buffer_enlarge_by(unsigned, auto& s) {
    eagitest::case_ test{s, 4, "enlarge by"};
    eagitest::track trck{test, 0, 2};
    auto& rg{test.random()};
    using namespace eagine;

    memory::buffer buf;

    int r = 0;

    span_size_t total{0};
    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto sz = span_size_t(r);

        buf.enlarge_by(sz);
        total += sz;

        test.check_equal(buf.size(), total, "size is ok");
        test.check(buf.capacity() >= total, "capacity is ok");

        memory::block blk = buf;

        if(buf.size() > 0) {
            test.check(bool(blk), "is true");
            test.check(!!blk, "is not false");
            test.check(not blk.empty(), "is not empty");
            test.check(blk.size() >= buf.size(), "size is ok");
            test.check(blk.begin() != blk.end(), "begin != end");
            trck.passed_part(1);
        } else {
            test.check(not bool(blk), "is false");
            test.check(not blk, "is not true");
            test.check(blk.empty(), "is empty");
            test.check(blk.begin() == blk.end(), "begin == end");
            trck.passed_part(2);
        }

        r = rg.get_int(0, 1000);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "buffer", 4};
    test.once(buffer_default_construct);
    test.repeat(10, buffer_resize);
    test.repeat(10, buffer_ensure);
    test.repeat(10, buffer_enlarge_by);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
