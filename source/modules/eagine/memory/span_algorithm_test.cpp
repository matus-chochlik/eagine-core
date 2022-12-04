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
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "span_algorithm", 12};
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
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
