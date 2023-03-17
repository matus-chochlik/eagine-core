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
//------------------------------------------------------------------------------
// equal
//------------------------------------------------------------------------------
template <typename T>
void span_equal_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<const T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    bool are_equal = (rng1.size() == rng2.size());

    for(span_size_t i = 0; i < rng1.size(); ++i) {
        are_equal &= (rng1[i] == rng2[i]);
    }

    test.check_equal(eagine::are_equal(rng1, rng2), are_equal, "are equal");
}
//------------------------------------------------------------------------------
template <typename T>
void span_equal_T_m_m_a(eagitest::case_& test, T min, T max, bool are) {
    auto& rg{test.random()};
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(v1.size());

    for(typename std::vector<T>::size_type i = 0; i < v1.size(); ++i) {
        v1[i] = rg.get_between<T>(min, max);
        if(are) {
            v2[i] = v1[i];
        } else {
            v2[i] = rg.get_between<T>(min, max);
        }
    }

    span_equal_T_r1_r2_1<T>(test, eagine::view(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_equal_T(eagitest::case_& test, T min, T max) {
    span_equal_T_m_m_a(test, min, max, true);
    span_equal_T_m_m_a(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_equal_1_char(auto& s) {
    eagitest::case_ test{s, 1, "span equal 1 char"};
    span_equal_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_equal_1_int(auto& s) {
    eagitest::case_ test{s, 2, "span equal 1 int"};
    span_equal_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// slice
//------------------------------------------------------------------------------
template <typename T>
void span_slice_r_p_s_1(
  eagitest::case_& test,
  eagine::span<const T> rng,
  eagine::span_size_t p,
  eagine::span_size_t s) {
    using namespace eagine;

    span<const T> slc = slice(rng, p, s);

    span_size_t n = slc.size();

    test.check(slc.size() <= rng.size(), "size 1");
    test.check(slc.size() <= s, "size 2");

    for(span_size_t i = 0; i < n; ++i) {
        test.check_equal(rng[p + i], slc[i], "element equal");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_slice_r_p_s_1(eagitest::case_& test, eagine::span<T> rng) {
    auto& rg{test.random()};
    using namespace eagine;

    span_slice_r_p_s_1(test, rng, 0, 0);
    span_slice_r_p_s_1(test, rng, 0, rng.size() / 2);
    span_slice_r_p_s_1(test, rng, 0, rng.size());
    span_slice_r_p_s_1(test, rng, 0, rng.size() * 2);

    span_slice_r_p_s_1(test, rng, rng.size() / 2, 0);
    span_slice_r_p_s_1(test, rng, rng.size() / 2, rng.size() / 2);
    span_slice_r_p_s_1(test, rng, rng.size() / 2, rng.size());
    span_slice_r_p_s_1(test, rng, rng.size() / 2, rng.size() * 2);

    span_slice_r_p_s_1(test, rng, rng.size() * 2, 0);
    span_slice_r_p_s_1(test, rng, rng.size() * 2, rng.size() / 2);
    span_slice_r_p_s_1(test, rng, rng.size() * 2, rng.size());
    span_slice_r_p_s_1(test, rng, rng.size() * 2, rng.size() * 2);

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        span_slice_r_p_s_1(
          test,
          rng,
          rg.get_span_size(0, rng.size() * 2),
          rg.get_span_size(0, rng.size() * 2));
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_slice_r_p_s_1(eagitest::case_& test, T min, T max) {
    auto& rg{test.random()};
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get_between<T>(min, max);
        }
        span_slice_r_p_s_1(test, eagine::view(v));
    }
}
//------------------------------------------------------------------------------
void span_slice_1_char(auto& s) {
    eagitest::case_ test{s, 3, "span slice 1 char"};
    span_slice_r_p_s_1<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_slice_1_int(auto& s) {
    eagitest::case_ test{s, 4, "span slice 1 int"};
    span_slice_r_p_s_1<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// slice 2
//------------------------------------------------------------------------------
template <typename T>
void span_slice_r_p_2(
  eagitest::case_& test,
  eagine::span<T> rng,
  eagine::span_size_t p) {
    using namespace eagine;

    span<T> slc = skip(rng, p);

    span_size_t n = slc.size();

    test.check(slc.size() <= rng.size(), "size");

    for(span_size_t i = 0; i < n; ++i) {
        test.check_equal(rng[p + i], slc[i], "element");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_slice_r_2(eagitest::case_& test, eagine::span<T> rng) {
    auto& rg{test.random()};
    using namespace eagine;

    span_slice_r_p_2(test, rng, 0);
    span_slice_r_p_2(test, rng, rng.size() / 2);
    span_slice_r_p_2(test, rng, rng.size());
    span_slice_r_p_2(test, rng, rng.size() * 2);

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        span_slice_r_p_2(test, rng, rg.get_span_size(0, rng.size() * 2));
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_slice_m_m_2(eagitest::case_& test, T min, T max) {
    auto& rg{test.random()};

    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get_between<T>(min, max);
        }
        span_slice_r_2(test, eagine::cover(v));
    }
}
//------------------------------------------------------------------------------
void span_slice_2_char(auto& s) {
    eagitest::case_ test{s, 5, "span slice 2 char"};
    span_slice_m_m_2<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_slice_2_int(auto& s) {
    eagitest::case_ test{s, 6, "span slice 2 int"};
    span_slice_m_m_2<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// split by
//------------------------------------------------------------------------------
template <typename T>
void span_split_by_r_p_s(
  eagitest::case_& test,
  eagine::span<const T> rng,
  eagine::span_size_t p,
  eagine::span_size_t s) {
    using namespace eagine;

    span<const T> sep{slice(rng, p, s)};
    auto [lf, rf] = memory::split_by_first(rng, sep);
    auto [ll, rl] = memory::split_by_last(rng, sep);

    test.check_equal(lf.size() + sep.size() + rf.size(), rng.size(), "size 1");
    test.check_equal(ll.size() + sep.size() + rl.size(), rng.size(), "size 2");
}
//------------------------------------------------------------------------------
template <typename T>
void span_split_by_r(eagitest::case_& test, eagine::span<const T> rng) {
    auto& rg{test.random()};
    using namespace eagine;

    span_split_by_r_p_s(test, rng, 0, 0);
    span_split_by_r_p_s(test, rng, 0, rng.size() / 2);
    span_split_by_r_p_s(test, rng, 0, rng.size());
    span_split_by_r_p_s(test, rng, 0, rng.size() * 2);

    span_split_by_r_p_s(test, rng, rng.size() / 2, 0);
    span_split_by_r_p_s(test, rng, rng.size() / 2, rng.size() / 2);
    span_split_by_r_p_s(test, rng, rng.size() / 2, rng.size());
    span_split_by_r_p_s(test, rng, rng.size() / 2, rng.size() * 2);

    span_split_by_r_p_s(test, rng, rng.size() * 2, 0);
    span_split_by_r_p_s(test, rng, rng.size() * 2, rng.size() / 2);
    span_split_by_r_p_s(test, rng, rng.size() * 2, rng.size());
    span_split_by_r_p_s(test, rng, rng.size() * 2, rng.size() * 2);

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        span_split_by_r_p_s(
          test,
          rng,
          rg.get_span_size(0, rng.size() * 2),
          rg.get_span_size(0, rng.size() * 2));
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_split_by_m_m(eagitest::case_& test, T min, T max) {
    auto& rg{test.random()};
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get_between<T>(min, max);
        }
        span_split_by_r(test, eagine::view(v));
    }
}
//------------------------------------------------------------------------------
void span_split_by_char(auto& s) {
    eagitest::case_ test{s, 7, "span split by char"};
    span_split_by_m_m<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_split_by_int(auto& s) {
    eagitest::case_ test{s, 8, "span split by int"};
    span_split_by_m_m<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// head
//------------------------------------------------------------------------------
template <typename T>
void span_head_r_s(
  eagitest::case_& test,
  eagine::span<T> rng,
  eagine::span_size_t l) {
    using namespace eagine;

    span<T> rng2 = head(rng, l);

    const span_size_t n = rng2.size();

    test.check(rng2.size() <= rng.size(), "size");

    for(span_size_t i = 0; i < n; ++i) {
        test.check_equal(rng[i], rng2[i], "element");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_head_r(eagitest::case_& test, eagine::span<T> rng) {
    auto& rg{test.random()};
    using namespace eagine;

    span_head_r_s(test, rng, 0);
    span_head_r_s(test, rng, rng.size() / 2);
    span_head_r_s(test, rng, rng.size());
    span_head_r_s(test, rng, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        span_head_r_s(test, rng, rg.get_span_size(0, rng.size() * 2));
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_head_m_m(eagitest::case_& test, T min, T max) {
    auto& rg{test.random()};
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get_between<T>(min, max);
        }
        span_head_r(test, eagine::cover(v));
    }
}
//------------------------------------------------------------------------------
void span_head_char(auto& s) {
    eagitest::case_ test{s, 9, "span head char"};
    span_head_m_m<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_head_int(auto& s) {
    eagitest::case_ test{s, 10, "span head int"};
    span_head_m_m<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// tail
//------------------------------------------------------------------------------
template <typename T>
void span_tail_r_s(
  eagitest::case_& test,
  eagine::span<T> rng,
  eagine::span_size_t l) {
    using namespace eagine;

    span<T> rng2 = tail(rng, l);

    const span_size_t n = rng2.size();

    test.check(rng2.size() <= rng.size(), "size");

    span_size_t p = rng.size() - l;
    if(p < 0) {
        p = 0;
    }

    for(span_size_t i = 0; i < n; ++i) {
        test.check_equal(rng[p + i], rng2[i], "element");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_tail_r(eagitest::case_& test, eagine::span<T> rng) {
    auto& rg{test.random()};
    using namespace eagine;

    span_tail_r_s(test, rng, 0);
    span_tail_r_s(test, rng, rng.size() / 2);
    span_tail_r_s(test, rng, rng.size());
    span_tail_r_s(test, rng, rng.size() * 2);

    for(int i = 0; i < 100; ++i) {
        span_tail_r_s(test, rng, rg.get_span_size(0, rng.size() * 2));
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_tail_m_m(eagitest::case_& test, T min, T max) {
    auto& rg{test.random()};
    for(int i = 0; i < 10; ++i) {
        std::vector<T> v(rg.get_std_size(20, 100));
        for(T& x : v) {
            x = rg.get_between<T>(min, max);
        }
        span_tail_r(test, eagine::cover(v));
    }
}
//------------------------------------------------------------------------------
void span_tail_char(auto& s) {
    eagitest::case_ test{s, 11, "span tail char"};
    span_tail_m_m<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_tail_int(auto& s) {
    eagitest::case_ test{s, 12, "span tail int"};
    span_tail_m_m<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// starts with
//------------------------------------------------------------------------------
template <typename T>
void span_starts_with_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<const T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    const span_size_t n = rng2.size();

    bool are_equal = true;

    for(span_size_t i = 0; i < n; ++i) {
        are_equal &= (rng1[i] == rng2[i]);
    }

    test.check_equal(eagine::are_equal(rng1, rng2), are_equal, "are equal");
}
//------------------------------------------------------------------------------
template <typename T>
void span_starts_with_T_m_m_a(eagitest::case_& test, T min, T max, bool does) {
    auto& rg{test.random()};
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(v1.size());

    for(typename std::vector<T>::size_type i = 0; i < v1.size(); ++i) {
        v1[i] = rg.get_between<T>(min, max);
        if(does) {
            v2[i] = v1[i];
        } else {
            v2[i] = rg.get_between<T>(min, max);
        }
    }

    span_starts_with_T_r1_r2_1<T>(test, eagine::view(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_starts_with_T(eagitest::case_& test, T min, T max) {
    span_starts_with_T_m_m_a(test, min, max, true);
    span_starts_with_T_m_m_a(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_starts_with_char(auto& s) {
    eagitest::case_ test{s, 13, "span starts with 1 char"};
    span_starts_with_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_starts_with_int(auto& s) {
    eagitest::case_ test{s, 14, "span starts with 1 int"};
    span_starts_with_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// ends with
//------------------------------------------------------------------------------
template <typename T>
void span_ends_with_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<T> rng1,
  eagine::span<const T> rng2) {
    using namespace eagine;

    const span_size_t n = rng2.size();

    const span_size_t p = rng1.size() - n;

    bool are_equal = true;

    for(span_size_t i = 0; i < n; ++i) {
        are_equal &= (rng1[p + i] == rng2[i]);
    }

    test.check_equal(ends_with(rng1, rng2), are_equal, "are equal");
}
//------------------------------------------------------------------------------
template <typename T>
void span_ends_with_T_m_m_a(eagitest::case_& test, T min, T max, bool does) {
    auto& rg{test.random()};
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;
    typename std::vector<T>::size_type p = v1.size() - v2.size();

    while(i < p) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get_between<T>(min, max);
        if(does) {
            v2[i - p] = v1[i];
        } else {
            v2[i - p] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    span_ends_with_T_r1_r2_1<T>(test, eagine::cover(v1), eagine::view(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_ends_with_T(eagitest::case_& test, T min, T max) {
    span_ends_with_T_m_m_a(test, min, max, true);
    span_ends_with_T_m_m_a(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_ends_with_char(auto& s) {
    eagitest::case_ test{s, 15, "span ends with 1 char"};
    span_ends_with_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_ends_with_int(auto& s) {
    eagitest::case_ test{s, 16, "span ends with 1 int"};
    span_ends_with_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// find position
//------------------------------------------------------------------------------
template <typename T>
void span_find_pos_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<const T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    if(const auto f{find_position(rng1, rng2)}) {
        span_size_t p = f.value();
        span_size_t n = rng2.size();

        bool are_equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            are_equal &= (rng1[p + i] == rng2[i]);
        }

        test.check(are_equal, "equal");
        test.check(contains(rng1, rng2), "contains");
    } else if(rng2.size() > 0) {
        test.check(not contains(rng1, rng2), "not contains");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_find_pos_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get_between<sz_t>(20, 100);
    sz_t l2 = rg.get_between<sz_t>(0, l1);
    sz_t p2 = rg.get_between<sz_t>(0, l1 - l2);
    sz_t i = 0;

    test.ensure(p2 + l2 <= l1, "position is ok");

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get_between<T>(min, max);
        if(has) {
            v2[i - p2] = v1[i];
        } else {
            v2[i - p2] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    span_find_pos_T_r1_r2_1(test, eagine::view(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_find_pos_T(eagitest::case_& test, T min, T max) {
    span_find_pos_T_m_m_h_1(test, min, max, true);
    span_find_pos_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_find_pos_char(auto& s) {
    eagitest::case_ test{s, 17, "span find position 1 char"};
    span_find_pos_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_find_pos_int(auto& s) {
    eagitest::case_ test{s, 18, "span find position 1 int"};
    span_find_pos_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// reverse find position
//------------------------------------------------------------------------------
template <typename T>
void span_rfind_pos_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<T> rng1,
  eagine::span<const T> rng2) {
    using namespace eagine;

    if(const auto f{reverse_find_position(rng1, rng2)}) {
        span_size_t p = f.value();
        span_size_t n = rng2.size();

        bool are_equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            are_equal &= (rng1[p + i] == rng2[i]);
        }
        test.check(are_equal, "equal");
        test.check(contains(rng1, rng2), "contains");
    } else if(rng2.size() > 0) {
        test.check(not contains(rng1, rng2), "not contains");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_rfind_pos_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get_between<sz_t>(20, 100);
    sz_t l2 = rg.get_between<sz_t>(0, l1);
    sz_t p2 = rg.get_between<sz_t>(0, l1 - l2);
    sz_t i = 0;

    test.ensure(p2 + l2 <= l1, "position is ok");

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get_between<T>(min, max);
        if(has) {
            v2[i - p2] = v1[i];
        } else {
            v2[i - p2] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    span_rfind_pos_T_r1_r2_1(test, eagine::cover(v1), eagine::view(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_rfind_pos_T(eagitest::case_& test, T min, T max) {
    span_rfind_pos_T_m_m_h_1(test, min, max, true);
    span_rfind_pos_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_rfind_pos_char(auto& s) {
    eagitest::case_ test{s, 19, "span reverse find position 1 char"};
    span_rfind_pos_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_rfind_pos_int(auto& s) {
    eagitest::case_ test{s, 20, "span reverse find position 1 int"};
    span_rfind_pos_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// find
//------------------------------------------------------------------------------
template <typename T>
void span_find_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    eagine::span<T> rng3 = find(rng1, rng2);
    if(rng3.size() > 0) {
        test.check(rng2.size() <= rng3.size(), "size");

        span_size_t n = rng2.size();

        bool are_equal = true;

        for(span_size_t i = 0; i < n; ++i) {
            are_equal &= (rng2[i] == rng3[i]);
        }

        test.check(are_equal, "equal");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_find_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    using sz_t = typename std::vector<T>::size_type;

    sz_t l1 = rg.get_between<sz_t>(20, 100);
    sz_t l2 = rg.get_between<sz_t>(0, l1);
    sz_t p2 = rg.get_between<sz_t>(0, l1 - l2);
    sz_t i = 0;

    test.ensure(p2 + l2 <= l1, "position is ok");

    std::vector<T> v1(l1);
    std::vector<T> v2(l2);

    while(i < p2) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    while(i < p2 + l2) {
        v1[i] = rg.get_between<T>(min, max);
        if(has) {
            v2[i - p2] = v1[i];
        } else {
            v2[i - p2] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    while(i < l1) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    span_find_T_r1_r2_1(test, eagine::cover(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_find_T(eagitest::case_& test, T min, T max) {
    span_find_T_m_m_h_1(test, min, max, true);
    span_find_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_find_char(auto& s) {
    eagitest::case_ test{s, 21, "span find element 1 char"};
    span_find_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_find_int(auto& s) {
    eagitest::case_ test{s, 22, "span find element 1 int"};
    span_find_T<int>(test, -10000, 10000);
}
//------------------------------------------------------------------------------
// find if
//------------------------------------------------------------------------------
template <typename T>
void span_find_if_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    const T mid = std::midpoint(min, max);

    std::vector<T> v;
    for(T cur = min; cur <= mid; ++cur) {
        v.push_back(cur);
    }

    for(unsigned i = 0; i < test.repeats(200); ++i) {
        if(has) {
            auto what = rg.get_between<T>(min, mid);
            auto pos = find_element_if(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.ensure(bool(pos), "position");
            test.check_equal(extract(pos), what - min, "position is ok");
        } else {
            auto what = rg.get_between<T>(mid + 1, max);
            auto pos = find_element_if(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.check(not pos, "not found");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_find_if_T(eagitest::case_& test, T min, T max) {
    span_find_if_T_m_m_h_1(test, min, max, true);
    span_find_if_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_find_if_char(auto& s) {
    eagitest::case_ test{s, 23, "span find if 1 char"};
    span_find_if_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_find_if_int(auto& s) {
    eagitest::case_ test{s, 24, "span find if 1 int"};
    span_find_if_T<int>(test, -1000, 1000);
}
//------------------------------------------------------------------------------
// take until
//------------------------------------------------------------------------------
template <typename T>
void span_take_until_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    const T mid = std::midpoint(min, max);

    std::vector<T> v;
    for(T cur = min; cur <= mid; ++cur) {
        v.push_back(cur);
    }

    test.check_equal(
      take_until(eagine::view(v), [min](auto elem) { return elem == min; })
        .size(),
      0,
      "front");
    test.check_equal(
      take_until(eagine::view(v), [mid](auto elem) { return elem == mid; })
        .size(),
      mid - min,
      "back");

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        if(has) {
            auto what = rg.get_between<T>(min, mid);
            auto spn = take_until(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.check_equal(spn.size(), what - min, "taken 1");
        } else {
            auto what = rg.get_between<T>(mid + 1, max);
            auto spn = take_until(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.check_equal(spn.size(), mid - min + 1, "taken 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_take_until_T(eagitest::case_& test, T min, T max) {
    span_take_until_T_m_m_h_1(test, min, max, true);
    span_take_until_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_take_until_char(auto& s) {
    eagitest::case_ test{s, 25, "span take until 1 char"};
    span_take_until_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_take_until_int(auto& s) {
    eagitest::case_ test{s, 26, "span take until 1 int"};
    span_take_until_T<int>(test, -1000, 1000);
}
//------------------------------------------------------------------------------
// skip until
//------------------------------------------------------------------------------
template <typename T>
void span_skip_until_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    const T mid = std::midpoint(min, max);

    std::vector<T> v;
    for(T cur = min; cur <= mid; ++cur) {
        v.push_back(cur);
    }

    test.check_equal(
      skip_until(eagine::view(v), [min](auto elem) { return elem == min; })
        .size(),
      mid - min + 1,
      "front");
    test.check_equal(
      skip_until(eagine::view(v), [mid](auto elem) { return elem == mid; })
        .size(),
      1,
      "back");

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        if(has) {
            auto what = rg.get_between<T>(min, mid);
            auto spn = skip_until(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.check_equal(spn.size(), mid - what + 1, "skipped");
        } else {
            auto what = rg.get_between<T>(mid + 1, max);
            auto spn = skip_until(
              eagine::view(v), [what](auto elem) { return elem == what; });
            test.check_equal(
              spn.size(), eagine::span_size(v.size()), "not skipped");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_skip_until_T(eagitest::case_& test, T min, T max) {
    span_skip_until_T_m_m_h_1(test, min, max, true);
    span_skip_until_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_skip_until_char(auto& s) {
    eagitest::case_ test{s, 27, "span skip until 1 char"};
    span_skip_until_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_skip_until_int(auto& s) {
    eagitest::case_ test{s, 28, "span skip until 1 int"};
    span_skip_until_T<int>(test, -1000, 1000);
}
//------------------------------------------------------------------------------
// strip prefix
//------------------------------------------------------------------------------
template <typename T>
void span_strip_prefix_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    bool empty_prefix = rng2.empty();
    bool had_prefix = starts_with(rng1, rng2);

    eagine::span<T> rng3 = strip_prefix(rng1, rng2);

    bool has_prefix = (rng3.size() == rng1.size()) and starts_with(rng3, rng2);

    if(had_prefix) {
        test.check(not has_prefix or empty_prefix, "prefix 1");
        test.check_equal(rng1.size(), rng2.size() + rng3.size(), "size 1");
    } else {
        test.check(not has_prefix, "prefix 2");
        test.check_equal(rng1.size(), rng3.size(), "size 2");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_strip_prefix_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;

    while(i < v2.size()) {
        v1[i] = rg.get_between<T>(min, max);
        if(has) {
            v2[i] = v1[i];
        } else {
            v2[i] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    span_strip_prefix_T_r1_r2_1(test, eagine::cover(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_strip_prefix_T(eagitest::case_& test, T min, T max) {
    span_strip_prefix_T_m_m_h_1(test, min, max, true);
    span_strip_prefix_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_strip_prefix_char(auto& s) {
    eagitest::case_ test{s, 29, "span strip prefix 1 char"};
    span_strip_prefix_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_strip_prefix_int(auto& s) {
    eagitest::case_ test{s, 30, "span strip prefix 1 int"};
    span_strip_prefix_T<int>(test, -1000, 1000);
}
//------------------------------------------------------------------------------
// strip suffix
//------------------------------------------------------------------------------
template <typename T>
void span_strip_suffix_T_r1_r2_1(
  eagitest::case_& test,
  eagine::span<T> rng1,
  eagine::span<T> rng2) {
    using namespace eagine;

    bool empty_suffix = rng2.empty();
    bool had_suffix = ends_with(rng1, rng2);

    eagine::span<T> rng3 = strip_suffix(rng1, rng2);

    bool has_suffix = (rng3.size() == rng1.size()) and ends_with(rng3, rng2);

    if(had_suffix) {
        test.check(not has_suffix or empty_suffix, "suffix 1");
        test.check_equal(rng1.size(), rng2.size() + rng3.size(), "size 1");
    } else {
        test.check(not has_suffix, "suffix 2");
        test.check_equal(rng1.size(), rng3.size(), "size 2");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void span_strip_suffix_T_m_m_h_1(eagitest::case_& test, T min, T max, bool has) {
    auto& rg{test.random()};
    std::vector<T> v1(rg.get_std_size(20, 100));
    std::vector<T> v2(rg.get_std_size(0, v1.size()));

    typename std::vector<T>::size_type i = 0;
    typename std::vector<T>::size_type p = v1.size() - v2.size();

    while(i < p) {
        v1[i] = rg.get_between<T>(min, max);
        ++i;
    }

    while(i < v1.size()) {
        v1[i] = rg.get_between<T>(min, max);
        if(has) {
            v2[i - p] = v1[i];
        } else {
            v2[i - p] = rg.get_between<T>(min, max);
        }
        ++i;
    }

    span_strip_suffix_T_r1_r2_1(test, eagine::cover(v1), eagine::cover(v2));
}
//------------------------------------------------------------------------------
template <typename T>
void span_strip_suffix_T(eagitest::case_& test, T min, T max) {
    span_strip_suffix_T_m_m_h_1(test, min, max, true);
    span_strip_suffix_T_m_m_h_1(test, min, max, false);
}
//------------------------------------------------------------------------------
void span_strip_suffix_char(auto& s) {
    eagitest::case_ test{s, 31, "span strip suffix 1 char"};
    span_strip_suffix_T<char>(test, 'A', 'Z');
}
//------------------------------------------------------------------------------
void span_strip_suffix_int(auto& s) {
    eagitest::case_ test{s, 32, "span strip suffix 1 int"};
    span_strip_suffix_T<int>(test, -1000, 1000);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "span_algorithm", 32};
    test.once(span_equal_1_char);
    test.once(span_equal_1_int);
    test.once(span_slice_1_char);
    test.once(span_slice_1_int);
    test.once(span_slice_2_char);
    test.once(span_slice_2_int);
    test.once(span_split_by_char);
    test.once(span_split_by_int);
    test.once(span_head_char);
    test.once(span_head_int);
    test.once(span_tail_char);
    test.once(span_tail_int);
    test.once(span_starts_with_char);
    test.once(span_starts_with_int);
    test.once(span_ends_with_char);
    test.once(span_ends_with_int);
    test.once(span_find_pos_char);
    test.once(span_find_pos_int);
    test.once(span_rfind_pos_char);
    test.once(span_rfind_pos_int);
    test.once(span_find_char);
    test.once(span_find_int);
    test.once(span_find_if_char);
    test.once(span_find_if_int);
    test.once(span_take_until_char);
    test.once(span_take_until_int);
    test.once(span_skip_until_char);
    test.once(span_skip_until_int);
    test.once(span_strip_prefix_char);
    test.once(span_strip_prefix_int);
    test.once(span_strip_suffix_char);
    test.once(span_strip_suffix_int);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
