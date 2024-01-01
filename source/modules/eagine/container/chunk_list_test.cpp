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
import eagine.core.container;
//------------------------------------------------------------------------------
void chunk_list_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::chunk_list<int, 32> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");
}
//------------------------------------------------------------------------------
void chunk_list_construct_resized(auto& s) {
    eagitest::case_ test{s, 2, "construct resized"};
    eagitest::track trck{test, 1, 1};
    eagine::chunk_list<int, 43> cl{10000};

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    std::generate(cl.begin(), cl.end(), [i{0}] mutable { return i++; });

    int i = 0;
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
void chunk_list_push_back(auto& s) {
    eagitest::case_ test{s, 3, "push back"};
    eagitest::track trck{test, 1, 1};
    eagine::chunk_list<int, 31> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
        trck.checkpoint(1);
    }
    test.check_equal(i - 1, cl.back(), "back is ok");
}
//------------------------------------------------------------------------------
void chunk_list_emplace_back(auto& s) {
    eagitest::case_ test{s, 4, "emplace back"};
    eagitest::track trck{test, 1, 1};
    eagine::chunk_list<int, 27> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.emplace_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok");
        trck.checkpoint(1);
    }
    test.check_equal(i - 1, cl.back(), "back is ok");
}
//------------------------------------------------------------------------------
void chunk_list_insert(auto& s) {
    eagitest::case_ test{s, 5, "insert"};
    eagitest::track trck{test, 2, 2};
    eagine::chunk_list<int, 23> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    auto ipos{cl.begin()};

    for(int i = 0; i < 10000; ++i) {
        ipos = cl.insert(ipos, i);
        ++ipos;
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok 1");
    }
    test.check_equal(i - 1, cl.back(), "back is ok");

    ipos = cl.begin();
    for(int j = 0; j < 10000; ++j) {
        ipos = cl.insert(ipos, j);
        ipos += 2;
        trck.checkpoint(1);
    }

    i = 0;
    for(const auto e : cl) {
        test.check_equal(i++ / 2, e, "element is ok 2");
        trck.checkpoint(2);
    }
}
//------------------------------------------------------------------------------
void chunk_list_emplace(auto& s) {
    eagitest::case_ test{s, 6, "emplace"};
    eagitest::track trck{test, 3, 3};
    eagine::chunk_list<int, 23> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    auto ipos{cl.begin()};

    for(int i = 0; i < 10000; ++i) {
        ipos = cl.emplace(ipos, i);
        ++ipos;
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 0;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        test.check_equal(i++, e, "element is ok 1");
        trck.checkpoint(1);
    }
    test.check_equal(i - 1, cl.back(), "back is ok");

    ipos = cl.begin();
    for(int j = 0; j < 10000; ++j) {
        ipos = cl.emplace(ipos, j);
        ipos += 2;
    }

    i = 0;
    for(const auto e : cl) {
        test.check_equal(i++ / 2, e, "element is ok 2");
        trck.checkpoint(2);
    }

    i = 0;
    for(auto it{cl.begin()}; it != cl.end(); it += 2) {
        test.check_equal(i++, *it, "element is ok 3");
        trck.checkpoint(3);
    }
}
//------------------------------------------------------------------------------
void chunk_list_resize(auto& s) {
    eagitest::case_ test{s, 7, "resize"};
    eagitest::track trck{test, 1, 3};
    eagine::chunk_list<int, 53> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    auto gen{[i{0}] mutable {
        return i++;
    }};

    for(int j = 0; j < 100; ++j) {
        cl.resize(cl.size() + 1);
        cl.back() = gen();
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 100U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    for(int j = 0; j < 200; ++j) {
        const auto ofs{cl.size()};
        cl.resize(ofs + j);
        for(auto it{cl.begin() + ofs}; it != cl.end(); ++it) {
            *it = gen();
        }
    }

    test.check(not cl.empty(), "is not empty");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    // int i = 0;
    for(const auto e : cl) {
        (void)e;
        // test.check_equal(i++, e, "element is ok 1");
        trck.checkpoint(1);
    }

    for(int j = 0; j < 200; ++j) {
        cl.resize(cl.size() - j);

        // i = 0;
        for(const auto e : cl) {
            (void)e;
            // test.check_equal(i++, e, "element is ok 2");
            trck.checkpoint(2);
        }
    }

    while(not cl.empty()) {
        cl.resize(cl.size() - 1);

        // i = 0;
        for(const auto e : cl) {
            (void)e;
            // test.check_equal(i++, e, "element is ok 3");
            trck.checkpoint(3);
        }
    }
}
//------------------------------------------------------------------------------
void chunk_list_erase(auto& s) {
    eagitest::case_ test{s, 8, "erase"};
    eagitest::track trck{test, 2, 2};
    eagine::chunk_list<int, 19> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.emplace_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    for(auto it{cl.begin()}; it != cl.end();) {
        if(*it % 3 == 0) {
            it = cl.erase(it);
        } else {
            ++it;
        }
        trck.checkpoint(1);
    }

    test.check(not cl.empty(), "is not empty");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    int i = 1;
    test.check_equal(i, cl.front(), "front is ok");
    for(const auto e : cl) {
        if(i % 3 != 0) {
            test.check_equal(i, e, "element is ok");
        } else {
            i++;
        }
        i++;
        trck.checkpoint(2);
    }
    test.check_equal(i - 1, cl.back(), "back is ok");
}
//------------------------------------------------------------------------------
void chunk_list_clear(auto& s) {
    eagitest::case_ test{s, 9, "clear"};
    eagine::chunk_list<int, 17> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() == cl.end(), "begin == end");

    for(int i = 0; i < 10000; ++i) {
        cl.emplace_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    cl.clear();

    for(const auto e : cl) {
        (void)e;
        test.fail("no elements in empty list");
    }

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() == cl.end(), "begin == end");
}
//------------------------------------------------------------------------------
void chunk_list_pop_back(auto& s) {
    eagitest::case_ test{s, 10, "popback"};
    eagitest::track trck{test, 1, 1};
    eagine::chunk_list<int, 13> cl;

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");

    int i = 0;
    for(; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    while(not cl.empty()) {
        test.check_equal(0, cl.front(), "front is ok");
        test.check_equal(--i, cl.back(), "back is ok");
        cl.pop_back();
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
void chunk_list_stack(auto& s) {
    eagitest::case_ test{s, 11, "stack"};
    eagitest::track trck{test, 1, 1};
    std::stack<int, eagine::chunk_list<int, 33>> stk;

    test.check(stk.empty(), "is empty");
    test.check_equal(stk.size(), 0U, "size is zero");

    int i = 0;
    for(; i < 10000; ++i) {
        stk.push(i);
    }

    test.check(not stk.empty(), "is not empty");
    test.check_equal(stk.size(), 10000U, "size is ok");

    while(not stk.empty()) {
        test.check_equal(--i, stk.top(), "top is ok");
        stk.pop();
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
void chunk_list_distance(auto& s) {
    eagitest::case_ test{s, 12, "distance"};
    eagitest::track trck{test, 1, 1};
    eagine::chunk_list<int, 111> cl;

    int i = 0;
    for(; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    i = 0;
    for(auto it{cl.begin()}; it != cl.end(); ++it, ++i) {
        test.check_equal(cl.begin() - it, -i, "dist to begin ok");
        test.check_equal(it - cl.begin(), i, "dist from begin ok");
        test.check_equal(cl.end() - it, 10000 - i, "dist to end ok");
        test.check_equal(it - cl.end(), i - 10000, "dist from end ok");
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
void chunk_list_lower_bound(auto& s) {
    eagitest::case_ test{s, 13, "lower_bound"};
    eagitest::track trck{test, 1, 2};
    eagine::chunk_list<int, 41> cl;

    int i = 0;
    for(; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(cl.begin() != cl.end(), "begin != end");

    for(i = -1; i <= 10000; ++i) {
        const auto pos{lower_bound(cl.begin(), cl.end(), i)};
        if(pos != cl.end()) {
            test.check(*pos >= i, "value is ok");
            trck.checkpoint(1);
        } else {
            trck.checkpoint(2);
        }
    }
}
//------------------------------------------------------------------------------
void chunk_list_upper_bound(auto& s) {
    eagitest::case_ test{s, 14, "upper_bound"};
    eagitest::track trck{test, 1, 2};
    eagine::chunk_list<int, 43> cl;

    int i = 0;
    for(; i < 10000; ++i) {
        cl.push_back(i);
    }

    test.check(cl.begin() != cl.end(), "begin != end");

    for(i = -1; i <= 10000; ++i) {
        const auto pos{upper_bound(cl.begin(), cl.end(), i)};
        if(pos != cl.end()) {
            test.check(i < *pos, "value is ok");
            trck.checkpoint(1);
        } else {
            trck.checkpoint(2);
        }
    }
}
//------------------------------------------------------------------------------
void chunk_list_random_insert_erase(auto& s) {
    eagitest::case_ test{s, 15, "random insert erase"};
    eagitest::track trck{test, 2, 2};
    eagine::chunk_list<int, 61> cl;

    std::vector<int> vec;
    vec.resize(10000U);
    std::generate(vec.begin(), vec.end(), [i{0}] mutable { return i++; });

    std::shuffle(
      vec.begin(),
      vec.end(),
      std::default_random_engine{std::random_device{}()});

    for(const auto i : vec) {
        using std::lower_bound;
        cl.insert(lower_bound(cl.begin(), cl.end(), i), i);
        test.check(std::is_sorted(cl.begin(), cl.end()), "is sorted insert");
        trck.checkpoint(1);
    }

    test.check(not cl.empty(), "is not empty");
    test.check_equal(cl.size(), 10000U, "size is ok");
    test.check(cl.size() <= cl.capacity(), "capacity is ok");
    test.check(cl.begin() != cl.end(), "begin != end");

    while(not vec.empty()) {
        const auto pos{std::find(cl.begin(), cl.end(), vec.back())};
        cl.erase(pos);
        vec.pop_back();
        test.check(std::is_sorted(cl.begin(), cl.end()), "is sorted erase");
        trck.checkpoint(2);
    }

    test.check(cl.empty(), "is empty");
    test.check_equal(cl.size(), 0U, "size is zero");
    test.check(cl.begin() == cl.end(), "begin == end");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "chunk_list", 15};
    test.once(chunk_list_default_construct);
    test.once(chunk_list_construct_resized);
    test.once(chunk_list_push_back);
    test.once(chunk_list_emplace_back);
    test.once(chunk_list_insert);
    test.once(chunk_list_emplace);
    test.once(chunk_list_resize);
    test.once(chunk_list_erase);
    test.once(chunk_list_clear);
    test.once(chunk_list_pop_back);
    test.once(chunk_list_stack);
    test.once(chunk_list_distance);
    test.once(chunk_list_lower_bound);
    test.once(chunk_list_upper_bound);
    test.once(chunk_list_random_insert_erase);

    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
