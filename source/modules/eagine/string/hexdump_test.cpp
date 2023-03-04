/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.string;
import eagine.core.memory;
import std;
//------------------------------------------------------------------------------
void hexdump_ostream(auto& s) {
    eagitest::case_ test{s, 1, "ostream"};
    using namespace eagine;

    const char buf[] = "hexdump test 1234567890";

    std::stringstream out;

    out << hexdump(as_bytes(view(buf)));

    test.check(not out.str().empty(), "not empty");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "hexdump", 1};
    test.once(hexdump_ostream);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
