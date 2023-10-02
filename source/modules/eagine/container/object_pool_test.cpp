/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.container;
//------------------------------------------------------------------------------
void object_pool_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::object_pool<std::string, 2> op;

    test.check(op.empty(), "is empty");
    test.check_equal(op.size(), 0U, "size is zero");
    test.check(op.size() <= op.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_make(auto& s) {
    eagitest::case_ test{s, 2, "make"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 16> op;

    std::vector<std::reference_wrapper<std::string>> refs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        refs.push_back(op.make(rg.get_string(2, 64)));
    }

    test.check(not op.empty(), "is not empty");
    test.check(op.size() > 0U, "size is not zero");
    test.check_equal(op.size(), refs.size(), "size is ok");
    test.check(op.size() <= op.capacity(), "size <= capacity");

    std::default_random_engine re{};
    std::shuffle(refs.begin(), refs.end(), re);

    for(const auto& ref : refs) {
        test.check(op.eat(ref.get()), "eat ok");
    }

    test.check(op.empty(), "is empty");
    test.check_equal(op.size(), 0U, "size is zero");
    test.check(op.size() <= op.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_get(auto& s) {
    eagitest::case_ test{s, 3, "get"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 32> op;

    std::vector<std::reference_wrapper<std::string>> refs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        refs.push_back(op.get().assign(rg.get_string(2, 64)));
    }

    test.check(not op.empty(), "is not empty");
    test.check(op.size() > 0U, "size is not zero");
    test.check_equal(op.size(), refs.size(), "size is ok");
    test.check(op.size() <= op.capacity(), "size <= capacity");

    std::default_random_engine re{};
    std::shuffle(refs.begin(), refs.end(), re);

    for(const auto& ref : refs) {
        test.check(op.eat(ref.get()), "eat ok");
    }

    test.check(op.empty(), "is empty");
    test.check_equal(op.size(), 0U, "size is zero");
    test.check(op.size() <= op.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_do_not_eat(auto& s) {
    eagitest::case_ test{s, 4, "don't eat"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 8> op;

    std::vector<std::string> strs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        strs.push_back(rg.get_string(2, 64));
    }

    for(const auto& str : strs) {
        test.check(not op.eat(str), "eat ok");
    }

    test.check(op.empty(), "is empty");
    test.check_equal(op.size(), 0U, "size is zero");
    test.check(op.size() <= op.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "object_pool", 4};
    test.once(object_pool_default_construct);
    test.once(object_pool_make);
    test.once(object_pool_get);
    test.once(object_pool_do_not_eat);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
