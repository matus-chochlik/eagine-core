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
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "span_algorithm", 4};
    test.once(span_equal_1_char);
    test.once(span_equal_1_int);
    test.once(span_slice_1_char);
    test.once(span_slice_1_int);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
