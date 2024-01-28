/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
//------------------------------------------------------------------------------
void double_buffer_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    eagine::double_buffer<int> db{1};
    const auto& cdb{db};

    test.check_equal(cdb.current(), 1, "current 1 A");
    test.check_equal(cdb.next(), 1, "next 1 A");
    db.swap();
    test.check_equal(db.current(), 1, "current 1 B");
    test.check_equal(db.next(), 1, "next 1 B");

    db.next() = 2;
    test.check_equal(db.front(), 1, "front 1 C");
    test.check_equal(db.back(), 2, "back 2 C");
    db.swap();
    test.check_equal(db.front(), 2, "front 2 D");
    test.check_equal(db.back(), 1, "back 1 D");

    db.back() = 2;
    test.check_equal(cdb.front(), 2, "front 2 E");
    test.check_equal(cdb.back(), 2, "back 2 E");
    db.front() = 3;
    db.swap();
    test.check_equal(cdb.current(), 2, "front 2 F");
    test.check_equal(cdb.next(), 3, "back 3 F");
}
//------------------------------------------------------------------------------
void double_buffer_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};

    eagine::double_buffer<int> db{1, 2};
    const auto& cdb{db};

    test.check_equal(cdb.current(), 1, "current 1 A");
    test.check_equal(cdb.next(), 2, "next 2 A");
    db.swap();
    test.check_equal(db.current(), 2, "current 2 B");
    test.check_equal(db.next(), 1, "next 1 B");

    db.next() = 3;
    test.check_equal(db.front(), 2, "front 2 C");
    test.check_equal(db.back(), 3, "back 3 C");
    db.swap();
    test.check_equal(db.front(), 3, "front 3 D");
    test.check_equal(db.back(), 2, "back 2 D");

    db.back() = 4;
    test.check_equal(cdb.front(), 3, "front 3 E");
    test.check_equal(cdb.back(), 4, "back 4 E");
    db.current() = 5;
    db.swap();
    test.check_equal(cdb.current(), 4, "front 4 F");
    test.check_equal(cdb.next(), 5, "back 5 F");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "double buffer", 2};
    test.once(double_buffer_1);
    test.once(double_buffer_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
