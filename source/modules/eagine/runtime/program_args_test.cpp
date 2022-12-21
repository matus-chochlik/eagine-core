/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.memory;
import eagine.core.runtime;
import <array>;
import <vector>;
import <sstream>;
//------------------------------------------------------------------------------
void program_args_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    std::stringstream out;

    std::array<const char*, 1> argv{{nullptr}};
    eagine::program_args args{0, argv.data()};
    test.constructed(args, "args");

    test.check(not args.command(), "no command");
    test.check(not args.command().is_valid(), "command not valid");
    test.check(args.none(), "none");
    test.check(args.empty(), "empty");
    test.check_equal(args.size(), 0, "size");
    test.check(args.begin() == args.end(), "begin == end");
    test.check(not args.first(), "not first");
    test.check(not args.first().is_valid(), "first not valid");
    test.check(not args.find(""), "not found empty");
    test.check(not args.find("eagitest"), "not found eagitest");
    test.check_equal(
      args.find_value_or("test", 1234, out), 1234, "find fallback");
}
//------------------------------------------------------------------------------
void program_args_none(auto& s) {
    eagitest::case_ test{s, 2, "none"};
    std::stringstream out;

    std::array<const char*, 2> argv{{"eagitest", nullptr}};
    eagine::program_args args{1, argv.data()};
    test.constructed(args, "args");

    test.check(bool(args.command()), "command true");
    test.check(args.command() == "eagitest", "command ok");
    test.check(args.command().is_valid(), "command is valid");
    test.check(args.none(), "none");
    test.check(not args.empty(), "not empty");
    test.check_equal(args.size(), 1, "size");
    test.check(args.begin() == args.end(), "begin == end");
    test.check(not args.first(), "not first");
    test.check(not args.first().is_valid(), "first not valid");
    test.check(not args.find(""), "not found empty");
    test.check(not args.find("eagitest"), "not found eagitest");
    test.check_equal(
      args.find_value_or("test", 1234, out), 1234, "find fallback");
}
//------------------------------------------------------------------------------
void program_args_some(auto& s) {
    eagitest::case_ test{s, 3, "some"};
    std::stringstream out;

    std::array<const char*, 4> argv{{"eagitest", "--int", "2345", nullptr}};
    eagine::program_args args{3, argv.data()};
    test.constructed(args, "args");

    test.check(bool(args.command()), "command true");
    test.check(args.command() == "eagitest", "command ok");
    test.check_equal(
      args.command().get(), eagine::string_view{"eagitest"}, "command ok");
    test.check(not args.none(), "none");
    test.check(not args.empty(), "not empty");
    test.check_equal(args.size(), 3, "size");
    test.check(args.begin() != args.end(), "begin != end");
    test.check(bool(args.first()), "first");
    test.check(args.first() == args.get(1), "first == get(1)");
    test.check(args.get(1) == args[1], "get(1) == [1]");
    test.check(args.get(2) == args[2], "get(2) == [2]");
    test.check(bool(args.find("--int")), "found arg");
    test.check_equal(args.find("--int").position(), 1, "found arg position");
    test.check_equal(args.find("2345").position(), 2, "found arg position");
    test.check(not args.find(""), "not found empty");
    test.check(not args.find("eagitest"), "not found eagitest");
    test.check_equal(
      args.find_value_or("--int", 1234, out), 2345, "find value");
}
//------------------------------------------------------------------------------
void program_args_iterator(auto& s) {
    eagitest::case_ test{s, 4, "iterator"};

    std::array<const char*, 6> argv{
      {"eagitest", "--int", "2345", "--bool", "true", nullptr}};
    eagine::program_args args{5, argv.data()};
    test.constructed(args, "args");

    test.check((args.begin() + 0) != args.end(), "0 != end");
    test.check(*(args.begin() + 0) == "--int", "0");
    test.check((args.begin() + 1)->get() == "2345", "1");
    test.check(*(args.begin() + 2) == "--bool", "2");
    test.check((args.begin() + 3)->get() == "true", "3");
    test.check((args.begin() + 4) == args.end(), "4 == end");

    auto pos = args.begin();
    test.check(pos != args.end(), "p0 != end");
    test.check(pos == (args.begin() + 0), "pp0");
    test.check(*pos == "--int", "p0");
    ++pos;
    test.check(pos == (args.begin() + 1), "pp1");
    test.check(pos != args.end(), "p1 != end");
    test.check(*pos == "2345", "p1");

    const auto prev{pos++};
    test.check(pos == (args.begin() + 2), "pp2");
    test.check(prev != args.end(), "r1 != end");
    test.check(*prev == "2345", "r1");
    test.check(*pos != "2345", "p2");

    const auto next{pos--};
    test.check(*pos == "2345", "p1");
    test.check(pos == (args.begin() + 1), "pp1");
    test.check(*next == "--bool", "r3");

    test.check(prev != next, "prev != next");
    test.check(prev < next, "prev < next");
    test.check(next > prev, "prev > next");
    test.check(prev <= next, "prev <= next");
    test.check(next >= prev, "next >= prev");
    test.check(next != prev, "next != prev");

    --pos;
    test.check(pos == (args.begin() + 0), "pp4");
    pos += 3;
    pos += 1;
    test.check(pos == args.end(), "p == end");
    pos -= 2;
    pos -= 2;
    pos -= 0;
    test.check(pos == args.begin(), "p == begin");
}
//------------------------------------------------------------------------------
void program_args_range_for(auto& s) {
    eagitest::case_ test{s, 5, "range for"};
    eagitest::track trck{test, 12, 1};

    std::array<const char*, 8> argv{
      {"eagitest",
       "--int",
       "2345",
       "--bool",
       "true",
       "--float",
       "1.234",
       nullptr}};
    eagine::program_args args{7, argv.data()};
    test.constructed(args, "args");

    int i = 1;
    for(auto& arg : args) {
        test.check(bool(arg), "bool");
        test.check(arg.is_valid(), "valid");
        test.check_equal(arg.position(), i++, "position");
        trck.passed_part(1);
    }

    i = 1;
    for(const auto& arg : static_cast<const eagine::program_args&>(args)) {
        test.check(bool(arg), "bool const");
        test.check(arg.is_valid(), "valid const");
        test.check_equal(arg.position(), i++, "position const");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
void program_args_parse(auto& s) {
    eagitest::case_ test{s, 6, "parse"};
    eagitest::track vect{test, 6, 1};
    std::stringstream out;

    std::array<const char*, 25> argv{
      {"eagitest", "--bool",  "true",  "--negative", "-5",     "--int",
       "6789",     "--ints",  "0",     "--ints",     "1",      "--ints",
       "2",        "--ints",  "3",     "--ints",     "4",      "--ints",
       "5",        "--float", "1.234", "--string",   "eagine", nullptr}};
    eagine::program_args args{int(argv.size()) - 1, argv.data()};
    test.constructed(args, "args");

    bool b{false};
    out = {};
    test.check(
      not args.find("--string").parse_next(b, out), "parse bool expect fail");
    test.check(not out.str().empty(), "parse bool expect fail message");
    out = {};
    test.check(args.find("--bool").parse_next(b, out), "parse bool");
    test.check(out.str().empty(), "parse bool message empty");
    test.check_equal(b, true, "parse bool result ok");

    int i{0};
    out = {};
    test.check(
      not args.find("--string").parse_next(i, out), "parse int expect fail");
    test.check(not out.str().empty(), "parse int expect fail message");
    out = {};
    test.check(args.find("--negative").parse_next(i, out), "parse int");
    test.check(out.str().empty(), "parse int message empty");
    test.check_equal(i, -5, "parse int result ok");
    out = {};
    test.check(args.find("--int").parse_next(i, out), "parse int");
    test.check(out.str().empty(), "parse int message empty");
    test.check_equal(i, 6789, "parse int result ok");

    float f{0};
    out = {};
    test.check(
      not args.find("--string").parse_next(f, out), "parse float expect fail");
    test.check(not out.str().empty(), "parse float expect fail message");
    out = {};
    test.check(args.find("--float").parse_next(f, out), "parse float");
    test.check(out.str().empty(), "parse float message empty");

    std::vector<int> v;
    out = {};
    for(const auto& arg : args) {
        if(arg == "--ints") {
            test.check(arg.parse_next(v, out), "parse int vector");
        }
    }
    test.check_equal(
      v.size(), std::size_t(6), "parse int vector size result ok");
    for(std::size_t idx = 0; idx < v.size(); ++idx) {
        test.check_equal(int(idx), v[idx], "parse vector element ok");
        vect.passed_part(1);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "program_args", 6};
    test.once(program_args_empty);
    test.once(program_args_none);
    test.once(program_args_some);
    test.once(program_args_iterator);
    test.once(program_args_range_for);
    test.once(program_args_parse);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
