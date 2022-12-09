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
import eagine.core.string;
import <stack>;
//------------------------------------------------------------------------------
void path_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    auto& rg{test.random()};
    using namespace eagine;

    basic_string_path bsp;

    for(unsigned i = 0; i < test.repeats(10); ++i) {
        span_size_t sz = 0;

        for(int j = 0, k = rg.get_int(10, 100); j < k; ++j) {
            const auto n{rg.get_string(0, 2048)};
            bsp.push_back(string_view(n));
            ++sz;

            test.check_equal(bsp.size(), sz, "path size 1");
            test.check_equal(
              bsp.back().size(), span_size(n.size()), "back size");
            test.check(are_equal(bsp.back(), string_view(n)), "back");

            if((rg.get_int(0, 9) == 0) && !bsp.empty()) {
                bsp.pop_back();
                --sz;
            }
            test.check_equal(bsp.size(), sz, "path size 2");
        }

        bsp = bsp + bsp;

        test.check_equal(bsp.size(), sz + sz, "path size 3");

        if(rg.get_bool()) {
            while(!bsp.empty()) {
                bsp.pop_back();
            }
        } else {
            bsp.clear();
        }

        test.check_equal(bsp.size(), 0, "path size 4");
    }
}
//------------------------------------------------------------------------------
void path_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};
    auto& rg{test.random()};
    using namespace eagine;

    for(unsigned i = 0; i < test.repeats(50); ++i) {
        basic_string_path bsp;

        for(int j = 0, k = rg.get_int(0, 30); j < k; ++j) {
            const auto str{rg.get_string(0, 2048)};
            bsp.push_back(string_view(str));

            test.check(are_equal(bsp.back(), string_view(str)), "bsp back");
            test.check(!bsp.empty(), "bsp not empty");
        }

        std::stack<std::string> stk;
        bsp.for_each([&](basic_string_path::value_type str) {
            stk.push(std::string(str.begin(), str.end()));
            test.check(are_equal(str, string_view(stk.top())), "stk top 1");
        });

        test.check_equal(bsp.size(), span_size(stk.size()), "size");

        bsp.rev_for_each([&](basic_string_path::value_type str) {
            test.ensure(!stk.empty(), "stk not empty");
            test.check(are_equal(str, string_view(stk.top())), "stk top 2");
            stk.pop();
        });

        test.check(stk.empty(), "stk empty");
    }
}
//------------------------------------------------------------------------------
void path_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};
    auto& rg{test.random()};
    using namespace eagine;

    for(unsigned i = 0; i < test.repeats(50); ++i) {
        basic_string_path bsp;

        for(int j = 0, k = rg.get_int(0, 30); j < k; ++j) {
            const auto str{rg.get_string(0, 2048)};
            bsp.push_back(string_view(str));

            test.check(are_equal(bsp.back(), string_view(str)), "bsp back");
            test.check(!bsp.empty(), "bsp not empty");
        }

        std::stack<std::string> stk;
        for(auto p = bsp.begin(); p != bsp.end(); ++p) {
            stk.push(std::string((*p).begin(), p->end()));
            test.check(are_equal(*p, string_view(stk.top())), "stk top");
        }

        test.check_equal(bsp.size(), span_size(stk.size()), "size");

        for(auto p = bsp.rbegin(); p != bsp.rend(); ++p) {
            test.ensure(!stk.empty(), "stk not empty");
            test.check(are_equal(*p, string_view(stk.top())), "dereference");
            stk.pop();
        }
        test.check(stk.empty(), "stk empty");
    }
}
//------------------------------------------------------------------------------
void path_4(auto& s) {
    eagitest::case_ test{s, 4, "4"};
    using namespace eagine;

    basic_string_path bsp;
    bsp.push_back("");
    bsp.push_back("A");
    bsp.push_back("BC");
    bsp.push_back("DEF");
    bsp.push_back("GHIJ");
    bsp.push_back("KLMNO");

    test.check_equal(bsp.as_string("", true), "ABCDEFGHIJKLMNO", "A");
    test.check_equal(bsp.as_string("", false), "ABCDEFGHIJKLMNO", "B");

    test.check_equal(bsp.as_string(":", true), ":A:BC:DEF:GHIJ:KLMNO:", "C");
    test.check_equal(bsp.as_string("|", false), "|A|BC|DEF|GHIJ|KLMNO", "D");

    test.check_equal(
      bsp.as_string("..", true), "..A..BC..DEF..GHIJ..KLMNO..", "E");
    test.check_equal(
      bsp.as_string("::", false), "::A::BC::DEF::GHIJ::KLMNO", "F");
}
//------------------------------------------------------------------------------
void path_5(auto& s) {
    eagitest::case_ test{s, 5, "5"};
    using namespace eagine;

    basic_string_path bsp("/usr/local/bin", split_by, "/");

    test.ensure(!bsp.empty(), "not empty");
    test.check_equal(bsp.size(), 4, "size");
    test.check(are_equal(bsp.front(), string_view("")), "front");
    test.check(are_equal(bsp.back(), string_view("bin")), "back");

    test.check_equal(
      bsp.as_string("::", false), "::usr::local::bin", "as str 1");

    test.check_equal(bsp.as_string(":", true), ":usr:local:bin:", "as str 2");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "path", 5};
    test.once(path_1);
    test.once(path_2);
    test.once(path_3);
    test.once(path_4);
    test.once(path_5);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
