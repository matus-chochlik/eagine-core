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
    eagine::object_pool<std::string, 2> pool;

    test.check(pool.empty(), "is empty");
    test.check_equal(pool.size(), 0U, "size is zero");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_make(auto& s) {
    eagitest::case_ test{s, 2, "make"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 16> pool;

    std::vector<std::reference_wrapper<std::string>> refs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        refs.push_back(pool.make(rg.get_string(2, 64)));
    }

    test.check(not pool.empty(), "is not empty");
    test.check(pool.size() > 0U, "size is not zero");
    test.check_equal(pool.size(), refs.size(), "size is ok");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");

    std::default_random_engine re{};
    std::shuffle(refs.begin(), refs.end(), re);

    for(const auto& ref : refs) {
        test.check(pool.eat(ref.get()), "eat ok");
    }

    test.check(pool.empty(), "is empty");
    test.check_equal(pool.size(), 0U, "size is zero");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_get(auto& s) {
    eagitest::case_ test{s, 3, "get"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 32> pool;

    std::vector<std::reference_wrapper<std::string>> refs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        refs.push_back(pool.get().assign(rg.get_string(2, 64)));
    }

    test.check(not pool.empty(), "is not empty");
    test.check(pool.size() > 0U, "size is not zero");
    test.check_equal(pool.size(), refs.size(), "size is ok");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");

    std::default_random_engine re{};
    std::shuffle(refs.begin(), refs.end(), re);

    for(const auto& ref : refs) {
        test.check(pool.eat(ref.get()), "eat ok");
    }

    test.check(pool.empty(), "is empty");
    test.check_equal(pool.size(), 0U, "size is zero");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_do_not_eat(auto& s) {
    eagitest::case_ test{s, 4, "don't eat"};
    auto& rg{test.random()};

    eagine::object_pool<std::string, 8> pool;

    std::vector<std::string> strs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        strs.push_back(rg.get_string(2, 64));
    }

    for(const auto& str : strs) {
        test.check(not pool.eat(str), "eat ok");
    }

    test.check(pool.empty(), "is empty");
    test.check_equal(pool.size(), 0U, "size is zero");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");
}
//------------------------------------------------------------------------------
void object_pool_object(auto& s) {
    eagitest::case_ test{s, 5, "object"};

    eagine::object_pool<std::string, 8> pool;
    std::vector<eagine::pool_object<std::string, 8>> strs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        strs.emplace_back(pool);
    }

    test.check(not pool.empty(), "is not empty");
    test.check_equal(pool.size(), strs.size(), "size is ok");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");

    strs.clear();

    test.check(pool.empty(), "is empty");
    test.check_equal(pool.size(), 0U, "size is zero");
    test.check(pool.size() <= pool.capacity(), "size <= capacity");
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
