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
//------------------------------------------------------------------------------
void interleaved_call_default_1(unsigned, auto& s) {
    eagitest::case_ test{s, 1, "1"};
    auto& rg{test.random()};

    std::string s1, s2;
    const std::string sep = rg.get_string(1, 5);

    auto fn = interleaved_call(
      [&s1](const std::string& s) { s1.append(s); },
      [&s1, &sep]() { s1.append(sep); });

    for(int j = 0, n = rg.get_between(1, 100); j < n; ++j) {
        const std::string s = rg.get_string(1, 10);

        fn(s);

        if(j > 0) {
            s2.append(sep);
        }
        s2.append(s);
    }

    test.check_equal(s1, s2, "are equal");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "interleaved_call", 1};
    test.repeat(100, interleaved_call_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
