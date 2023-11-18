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
import eagine.core.memory;
import eagine.core.string;
import eagine.core.runtime;
//------------------------------------------------------------------------------
void program_args_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};

    std::array<const char*, 1> argv{{nullptr}};
    eagine::program_args args{0, argv.data()};
    test.constructed(args, "args");

    test.check(not args.command(), "no command");
    test.check(not args.command().has_value(), "command not valid");
    test.check(args.none(), "none");
    test.check(args.empty(), "empty");
    test.check_equal(args.size(), 0, "size");
    test.check(args.begin() == args.end(), "begin == end");
    test.check(not args.first(), "not first");
    test.check(not args.first().has_value(), "first not valid");
    test.check(not args.find(""), "not found empty");
    test.check(not args.find("eagitest"), "not found eagitest");
}
//------------------------------------------------------------------------------
void program_args_none(auto& s) {
    eagitest::case_ test{s, 2, "none"};

    std::array<const char*, 2> argv{{"eagitest", nullptr}};
    eagine::program_args args{1, argv.data()};
    test.constructed(args, "args");

    test.check(bool(args.command()), "command true");
    test.check(args.command() == "eagitest", "command ok");
    test.check(args.command().has_value(), "command is valid");
    test.check(args.none(), "none");
    test.check(not args.empty(), "not empty");
    test.check_equal(args.size(), 1, "size");
    test.check(args.begin() == args.end(), "begin == end");
    test.check(not args.first(), "not first");
    test.check(not args.first().has_value(), "first not valid");
    test.check(not args.find(""), "not found empty");
    test.check(not args.find("eagitest"), "not found eagitest");
}
//------------------------------------------------------------------------------
void program_args_some(auto& s) {
    eagitest::case_ test{s, 3, "some"};

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
    eagitest::track trck{test, 12, 2};

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
        test.check(arg.has_value(), "valid");
        test.check_equal(arg.position(), i++, "position");
        trck.checkpoint(1);
    }

    i = 1;
    for(const auto& arg : static_cast<const eagine::program_args&>(args)) {
        test.check(bool(arg), "bool const");
        test.check(arg.has_value(), "valid const");
        test.check_equal(arg.position(), i++, "position const");
        trck.checkpoint(2);
    }
}
//------------------------------------------------------------------------------
void program_args_parse(auto& s) {
    eagitest::case_ test{s, 6, "parse"};

    std::array<const char*, 25> argv{
      {"eagitest", "--bool",  "true",  "--negative", "-5",     "--int",
       "6789",     "--ints",  "0",     "--ints",     "1",      "--ints",
       "2",        "--ints",  "3",     "--ints",     "4",      "--ints",
       "5",        "--float", "1.234", "--string",   "eagine", nullptr}};
    eagine::program_args args{int(argv.size()) - 1, argv.data()};
    test.constructed(args, "args");

    bool b{false};
    test.check(
      not assign_if_fits(args.find("--string").next(), b),
      "parse bool expect fail");
    test.check(assign_if_fits(args.find("--bool").next(), b), "parse bool");
    test.check_equal(b, true, "parse bool result ok");

    int i{0};
    test.check(
      not assign_if_fits(args.find("--string").next(), i),
      "parse int expect fail");
    test.check(assign_if_fits(args.find("--negative").next(), i), "parse int");
    test.check_equal(i, -5, "parse int result ok");
    test.check(assign_if_fits(args.find("--int").next(), i), "parse int");
    test.check_equal(i, 6789, "parse int result ok");

    float f{0};
    test.check(
      not assign_if_fits(args.find("--string").next(), f),
      "parse float expect fail");
    test.check(assign_if_fits(args.find("--float").next(), f), "parse float");
}
//------------------------------------------------------------------------------
void program_args_all_like(auto& s) {
    eagitest::case_ test{s, 7, "all_like"};

    std::array<const char*, 25> argv{
      {"eagitest", "--bool",  "true",  "--negative", "-5",     "--int",
       "6789",     "--ints",  "0",     "--ints",     "1",      "--ints",
       "2",        "--ints",  "3",     "--ints",     "4",      "--ints",
       "5",        "--float", "1.234", "--string",   "eagine", nullptr}};

    eagine::program_args args{int(argv.size()) - 1, argv.data()};

    eagitest::track ints{test, "ints", 5, 1};
    for(const auto arg : args.all_like("--ints")) {
        test.check(arg.has_value(), "--ints");
        ints.checkpoint(1);
    }

    eagitest::track floats{test, "floats", 1, 1};
    for(const auto arg : args.all_like("--float")) {
        test.check(arg.has_value(), "--float");
        floats.checkpoint(1);
    }

    for(const auto arg : args.all_like("--nonexistent")) {
        test.check(arg.has_value(), "--nonexistent");
        test.fail("should not get here");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "program_args", 7};
    test.once(program_args_empty);
    test.once(program_args_none);
    test.once(program_args_some);
    test.once(program_args_iterator);
    test.once(program_args_range_for);
    test.once(program_args_parse);
    test.once(program_args_all_like);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
