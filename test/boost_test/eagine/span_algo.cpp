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
void test_range_slice_r_p_1(eagine::span<T> rng, eagine::span_size_t p) {
    using namespace eagine;

    span<T> slc = skip(rng, p);

    span_size_t n = slc.size();

    BOOST_CHECK(slc.size() <= rng.size());

    for(span_size_t i = 0; i < n; ++i) {
        BOOST_CHECK_EQUAL(rng[p + i], slc[i]);
    }
}

template <typename T>
void test_range_slice_r_p_1(eagine::span<T> rng) {
    using namespace eagine;

    test_range_slice_r_p_1(rng, 0);
    test_range_slice_r_p_1(rng, rng.size() / 2);
    test_range_slice_r_p_1(rng, rng.size());
    test_range_slice_r_p_1(rng, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        test_range_slice_r_p_1(rng, rg.get_span_size(0, rng.size() * 2));
    }
}

template <typename T>
void test_range_slice_r_p_1(T min, T max) {
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get<T>(min, max);
        }
        test_range_slice_r_p_1(eagine::cover(v));
    }
}

BOOST_AUTO_TEST_CASE(span_algo_slice_r_p_1) {
    test_range_slice_r_p_1<char>('A', 'Z');
    test_range_slice_r_p_1<int>(-100000, 100000);
}

template <typename T>
void test_range_split_by(
  eagine::span<T> rng,
  eagine::span_size_t p,
  eagine::span_size_t s) {
    using namespace eagine;

    eagine::span<T> sep{slice(rng, p, s)};
    auto [lf, rf] = memory::split_by_first(rng, sep);
    auto [ll, rl] = memory::split_by_last(rng, sep);

    BOOST_CHECK_EQUAL(lf.size() + sep.size() + rf.size(), rng.size());
    BOOST_CHECK_EQUAL(ll.size() + sep.size() + rl.size(), rng.size());
}

template <typename T>
void test_range_split_by(eagine::span<T> rng) {
    using namespace eagine;

    test_range_split_by(rng, 0, 0);
    test_range_split_by(rng, 0, rng.size() / 2);
    test_range_split_by(rng, 0, rng.size());
    test_range_split_by(rng, 0, rng.size() * 2);

    test_range_split_by(rng, rng.size() / 2, 0);
    test_range_split_by(rng, rng.size() / 2, rng.size() / 2);
    test_range_split_by(rng, rng.size() / 2, rng.size());
    test_range_split_by(rng, rng.size() / 2, rng.size() * 2);

    test_range_split_by(rng, rng.size() * 2, 0);
    test_range_split_by(rng, rng.size() * 2, rng.size() / 2);
    test_range_split_by(rng, rng.size() * 2, rng.size());
    test_range_split_by(rng, rng.size() * 2, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        test_range_split_by(
          rng,
          rg.get_span_size(0, rng.size() * 2),
          rg.get_span_size(0, rng.size() * 2));
    }
}

template <typename T>
void test_range_split_by(T min, T max) {
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get<T>(min, max);
        }
        test_range_split_by(eagine::cover(v));
    }
}

BOOST_AUTO_TEST_CASE(span_algo_split_by) {
    test_range_split_by<char>('A', 'Z');
    test_range_split_by<int>(-100000, 100000);
}

template <typename T>
void test_range_head_1(eagine::span<T> rng, eagine::span_size_t l) {
    using namespace eagine;

    span<T> rng2 = head(rng, l);

    span_size_t n = rng2.size();

    BOOST_CHECK(rng2.size() <= rng.size());

    for(span_size_t i = 0; i < n; ++i) {
        BOOST_CHECK_EQUAL(rng[i], rng2[i]);
    }
}

template <typename T>
void test_range_head_1(eagine::span<T> rng) {
    using namespace eagine;

    test_range_head_1(rng, 0);
    test_range_head_1(rng, rng.size() / 2);
    test_range_head_1(rng, rng.size());
    test_range_head_1(rng, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        test_range_head_1(rng, rg.get_span_size(0, rng.size() * 2));
    }
}

template <typename T>
void test_range_head_1(T min, T max) {
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get<T>(min, max);
        }
        test_range_head_1(eagine::cover(v));
    }
}

BOOST_AUTO_TEST_CASE(span_algo_head_1) {
    test_range_head_1<char>('A', 'Z');
    test_range_head_1<int>(-100000, 100000);
}

template <typename T>
void test_range_tail_1(eagine::span<T> rng, eagine::span_size_t l) {
    using namespace eagine;

    span<T> rng2 = tail(rng, l);

    span_size_t n = rng2.size();

    BOOST_CHECK(rng2.size() <= rng.size());

    span_size_t p = rng.size() - l;
    if(p < 0)
        p = 0;

    for(span_size_t i = 0; i < n; ++i) {
        BOOST_CHECK_EQUAL(rng[p + i], rng2[i]);
    }
}

template <typename T>
void test_range_tail_1(eagine::span<T> rng) {
    using namespace eagine;

    test_range_tail_1(rng, 0);
    test_range_tail_1(rng, rng.size() / 2);
    test_range_tail_1(rng, rng.size());
    test_range_tail_1(rng, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        test_range_tail_1(rng, rg.get_span_size(0, rng.size() * 2));
    }
}

template <typename T>
void test_range_tail_1(T min, T max) {
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get<T>(min, max);
        }
        test_range_tail_1(eagine::cover(v));
    }
}

BOOST_AUTO_TEST_CASE(span_algo_tail_1) {
    test_range_tail_1<char>('A', 'Z');
    test_range_tail_1<int>(-100000, 100000);
}

template <typename T>
void test_range_starts_with_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    span_size_t n = rng2.size();

    bool equal = true;

    for(span_size_t i = 0; i < n; ++i) {
        equal &= (rng1[i] == rng2[i]);
    }

    BOOST_CHECK_EQUAL(starts_with(rng1, rng2), equal);
}

template <typename T>
void test_range_starts_with_1(T min, T max, bool does) {
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;

    while(i < v2.size()) {
        v1[i] = rg.get<T>(min, max);
        if(does)
            v2[i] = v1[i];
        else
            v2[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    test_range_starts_with_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_starts_with_1(T min, T max) {
    test_range_starts_with_1<T>(min, max, false);
    test_range_starts_with_1<T>(min, max, true);

    for(int i = 0; i < 10; ++i) {
        test_range_starts_with_1<T>(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_starts_with_1) {
    test_range_starts_with_1<char>('A', 'Z');
    test_range_starts_with_1<int>(-100000, 100000);
}

template <typename T>
void test_range_ends_with_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    span_size_t n = rng2.size();

    span_size_t p = rng1.size() - n;

    bool equal = true;

    for(span_size_t i = 0; i < n; ++i) {
        equal &= (rng1[p + i] == rng2[i]);
    }

    BOOST_CHECK_EQUAL(ends_with(rng1, rng2), equal);
}

template <typename T>
void test_range_ends_with_1(T min, T max, bool does) {
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
        if(does)
            v2[i - p] = v1[i];
        else
            v2[i - p] = rg.get<T>(min, max);
        ++i;
    }

    test_range_ends_with_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_ends_with_1(T min, T max) {
    test_range_ends_with_1<T>(min, max, false);
    test_range_ends_with_1<T>(min, max, true);

    for(int i = 0; i < 10; ++i) {
        test_range_ends_with_1<T>(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_ends_with_1) {
    test_range_ends_with_1<char>('A', 'Z');
    test_range_ends_with_1<int>(-100000, 100000);
}

template <typename T>
void test_range_find_pos_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    if(const auto f{find_position(rng1, rng2)}) {
        span_size_t p = f.value();
        span_size_t n = rng2.size();

        bool equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            equal &= (rng1[p + i] == rng2[i]);
        }

        BOOST_CHECK(equal);
        BOOST_CHECK(contains(rng1, rng2));
    } else if(rng2.size() > 0) {
        BOOST_CHECK(!contains(rng1, rng2));
    }
}

template <typename T>
void test_range_find_pos_1(T min, T max, bool has) {
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get<sz_t>(20, 100);
    sz_t l2 = rg.get<sz_t>(0, l1);
    sz_t p2 = rg.get<sz_t>(0, l1 - l2);
    sz_t i = 0;

    assert(p2 + l2 <= l1);

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get<T>(min, max);
        if(has)
            v2[i - p2] = v1[i];
        else
            v2[i - p2] = rg.get<T>(min, max);
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    test_range_find_pos_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_find_pos_1(T min, T max) {
    test_range_find_pos_1(min, max, true);
    test_range_find_pos_1(min, max, false);

    for(int i = 0; i < 10; ++i) {
        test_range_find_pos_1(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_find_pos_1) {
    test_range_find_pos_1<char>('A', 'Z');
    test_range_find_pos_1<int>(-100000, 100000);
}

template <typename T>
void test_range_rfind_pos_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    if(const auto f{reverse_find_position(rng1, rng2)}) {
        span_size_t p = f.value();
        span_size_t n = rng2.size();

        bool equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            equal &= (rng1[p + i] == rng2[i]);
        }

        BOOST_CHECK(equal);
        BOOST_CHECK(contains(rng1, rng2));
    } else if(rng2.size() > 0) {
        BOOST_CHECK(!contains(rng1, rng2));
    }
}

template <typename T>
void test_range_rfind_pos_1(T min, T max, bool has) {
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get<sz_t>(20, 100);
    sz_t l2 = rg.get<sz_t>(0, l1);
    sz_t p2 = rg.get<sz_t>(0, l1 - l2);
    sz_t i = 0;

    assert(p2 + l2 <= l1);

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get<T>(min, max);
        if(has)
            v2[i - p2] = v1[i];
        else
            v2[i - p2] = rg.get<T>(min, max);
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    test_range_rfind_pos_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_rfind_pos_1(T min, T max) {
    test_range_rfind_pos_1(min, max, true);
    test_range_rfind_pos_1(min, max, false);

    for(int i = 0; i < 10; ++i) {
        test_range_rfind_pos_1(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_rfind_pos_1) {
    test_range_rfind_pos_1<char>('A', 'Z');
    test_range_rfind_pos_1<int>(-100000, 100000);
}

template <typename T>
void test_range_find_1(eagine::span<T> rng1, eagine::span<T> rng2) {
    using namespace eagine;

    eagine::span<T> rng3 = find(rng1, rng2);
    if(rng3.size() > 0) {
        BOOST_ASSERT(rng2.size() <= rng3.size());

        span_size_t n = rng2.size();

        bool equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            equal &= (rng2[i] == rng3[i]);
        }

        BOOST_CHECK(equal);
    }
}

template <typename T>
void test_range_find_1(T min, T max, bool has) {
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get<sz_t>(20, 100);
    sz_t l2 = rg.get<sz_t>(0, l1);
    sz_t p2 = rg.get<sz_t>(0, l1 - l2);
    sz_t i = 0;

    assert(p2 + l2 <= l1);

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get<T>(min, max);
        if(has)
            v2[i - p2] = v1[i];
        else
            v2[i - p2] = rg.get<T>(min, max);
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get<T>(min, max);
        ++i;
    }

    test_range_find_1(eagine::cover(v1), eagine::cover(v2));
}

template <typename T>
void test_range_find_1(T min, T max) {
    test_range_find_1(min, max, true);
    test_range_find_1(min, max, false);

    for(int i = 0; i < 10; ++i) {
        test_range_find_1(min, max, rg.get_bool());
    }
}

BOOST_AUTO_TEST_CASE(span_algo_find_1) {
    test_range_find_1<char>('A', 'Z');
    test_range_find_1<int>(-100000, 100000);
}

template <typename T>
void test_range_find_if(T min, T max) {
    using sz_t = typename std::vector<T>::size_type;

    std::vector<T> v;
    for(T cur = min; cur <= max; ++cur) {
        v.push_back(cur);
    }

    for(int i = 0; i < 50; ++i) {
        auto what = rg.get<T>(min, max);
        auto pos = find_element_if(
          eagine::view(v), [what](auto elem) { return elem == what; });
        EAGINE_ASSERT(pos);
        BOOST_CHECK_EQUAL(extract(pos), what - min);
    }
}

BOOST_AUTO_TEST_CASE(span_algo_find_if) {
    test_range_find_if<char>('A', 'Z');
    test_range_find_if<int>(-1000, 1000);
}

template <typename T>
void test_range_take_until(T min, T max) {
    using sz_t = typename std::vector<T>::size_type;

    std::vector<T> v;
    for(T cur = min; cur <= max; ++cur) {
        v.push_back(cur);
    }

    BOOST_CHECK_EQUAL(
      take_until(eagine::view(v), [min](auto elem) { return elem == min; })
        .size(),
      0);
    BOOST_CHECK_EQUAL(
      take_until(eagine::view(v), [max](auto elem) { return elem == max; })
        .size(),
      max - min);

    for(int i = 0; i < 50; ++i) {
        auto what = rg.get<T>(min, max);
        auto spn = take_until(
          eagine::view(v), [what](auto elem) { return elem == what; });
        BOOST_CHECK_EQUAL(spn.size(), what - min);
    }
}

BOOST_AUTO_TEST_CASE(span_algo_take_until) {
    test_range_take_until<char>('A', 'Z');
    test_range_take_until<int>(-1000, 1000);
}

template <typename T>
void test_range_skip_until(T min, T max) {
    using sz_t = typename std::vector<T>::size_type;

    std::vector<T> v;
    for(T cur = min; cur <= max; ++cur) {
        v.push_back(cur);
    }

    BOOST_CHECK_EQUAL(
      skip_until(eagine::view(v), [min](auto elem) { return elem == min; })
        .size(),
      max - min + 1);
    BOOST_CHECK_EQUAL(
      skip_until(eagine::view(v), [max](auto elem) { return elem == max; })
        .size(),
      1);

    for(int i = 0; i < 50; ++i) {
        auto what = rg.get<T>(min, max);
        auto spn = skip_until(
          eagine::view(v), [what](auto elem) { return elem == what; });
        BOOST_CHECK_EQUAL(spn.size(), max - what + 1);
    }
}

BOOST_AUTO_TEST_CASE(span_algo_skip_until) {
    test_range_skip_until<char>('A', 'Z');
    test_range_skip_until<int>(-1000, 1000);
}

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
