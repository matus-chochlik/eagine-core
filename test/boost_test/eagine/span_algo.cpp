/*
 *  Copyright Matus Chochlik.
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */
#include <eagine/memory/span_algo.hpp>
#define BOOST_TEST_MODULE EAGINE_span_algo
#include "../unit_test_begin.inl"

#include <eagine/span.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(span_algo_tests)

static eagine::test_random_generator rg;

template <typename T>
void test_range_strip_prefix_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    bool empty_prefix = rng2.empty();
    bool had_prefix = starts_with(rng1, rng2);

    eagine::span<T> rng3 = strip_prefix(rng1, rng2);

    bool has_prefix = (rng3.size() == rng1.size()) && starts_with(rng3, rng2);

    if(had_prefix) {
        BOOST_CHECK(!has_prefix || empty_prefix);
        BOOST_CHECK_EQUAL(rng1.size(), rng2.size() + rng3.size());
    } else {
        BOOST_CHECK(!has_prefix);
        BOOST_CHECK_EQUAL(rng1.size(), rng3.size());
    }
}

template <typename T>
void test_range_strip_prefix_1(T min, T max, bool has) {
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;

    while(i < v2.size()) {
        v1[i] = rg.get<T>(min, max);
        if(has)
            v2[i] = v1[i];
        else
            v2[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    test_range_strip_prefix_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_strip_prefix_1(T min, T max) {
    test_range_strip_prefix_1(min, max, true);
    test_range_strip_prefix_1(min, max, false);

    for(int i = 0; i < 10; ++i) {
        test_range_strip_prefix_1(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_strip_prefix_1) {
    test_range_strip_prefix_1<char>('A', 'Z');
    test_range_strip_prefix_1<int>(-100000, 100000);
}

template <typename T>
void test_range_strip_suffix_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    bool empty_suffix = rng2.empty();
    bool had_suffix = ends_with(rng1, rng2);

    eagine::span<T> rng3 = strip_suffix(rng1, rng2);

    bool has_suffix = (rng3.size() == rng1.size()) && ends_with(rng3, rng2);

    if(had_suffix) {
        BOOST_CHECK(!has_suffix || empty_suffix);
        BOOST_CHECK_EQUAL(rng1.size(), rng2.size() + rng3.size());
    } else {
        BOOST_CHECK(!has_suffix);
        BOOST_CHECK_EQUAL(rng1.size(), rng3.size());
    }
}

template <typename T>
void test_range_strip_suffix_1(T min, T max, bool has) {
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;
    typename std::vector<T>::size_type p = v1.size() - v2.size();

    while(i < p) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get<T>(min, max);
        if(has)
            v2[i - p] = v1[i];
        else
            v2[i - p] = rg.get<T>(min, max);
        ++i;
    }

    test_range_strip_suffix_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_strip_suffix_1(T min, T max) {
    test_range_strip_suffix_1(min, max, true);
    test_range_strip_suffix_1(min, max, false);

    for(int i = 0; i < 10; ++i) {
        test_range_strip_suffix_1(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_strip_suffix_1) {
    test_range_strip_suffix_1<char>('A', 'Z');
    test_range_strip_suffix_1<int>(-100000, 100000);
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
