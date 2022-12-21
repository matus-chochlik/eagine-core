/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
//------------------------------------------------------------------------------
void double_buffer_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    eagine::double_buffer<int> db{1};
    const auto& cdb{db};

    test.check_equal(cdb.front(), 1, "front 1 A");
    test.check_equal(cdb.back(), 1, "back 1 A");
    db.swap();
    test.check_equal(db.front(), 1, "front 1 B");
    test.check_equal(db.back(), 1, "back 1 B");

    db.back() = 2;
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
    test.check_equal(cdb.front(), 2, "front 2 F");
    test.check_equal(cdb.back(), 3, "back 3 F");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "double buffer", 1};
    test.once(double_buffer_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
