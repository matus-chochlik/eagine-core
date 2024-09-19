/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
//------------------------------------------------------------------------------
// fill
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void simd_fill_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a = rg.get_any<T>();

        eagine::simd::data_t<T, N, V> v{eagine::simd::fill<T, N, V>::apply(a)};

        for(int i = 0; i < N; ++i) {
            test.check_equal(v[i], a, "equal");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void simd_fill_TV(eagitest::case_& test) {
    simd_fill_TNV<T, 2, V>(test);
    simd_fill_TNV<T, 3, V>(test);
    simd_fill_TNV<T, 4, V>(test);
    simd_fill_TNV<T, 5, V>(test);
    simd_fill_TNV<T, 6, V>(test);
    simd_fill_TNV<T, 7, V>(test);
    simd_fill_TNV<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void simd_fill_T(eagitest::case_& test) {
    simd_fill_TV<T, true>(test);
    simd_fill_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void simd_fill_byte(auto& s) {
    eagitest::case_ test{s, 1, "byte"};
    simd_fill_T<unsigned char>(test);
}
//------------------------------------------------------------------------------
void simd_fill_int(auto& s) {
    eagitest::case_ test{s, 2, "int"};
    simd_fill_T<int>(test);
}
//------------------------------------------------------------------------------
void simd_fill_float(auto& s) {
    eagitest::case_ test{s, 3, "float"};
    simd_fill_T<float>(test);
}
//------------------------------------------------------------------------------
void simd_fill_double(auto& s) {
    eagitest::case_ test{s, 4, "double"};
    simd_fill_T<double>(test);
}
//------------------------------------------------------------------------------
// is zero
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void simd_is_zero_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    const T z = T(0);
    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        const bool gen_zero = rg.get_int(0, 9) == 0;
        const T a = gen_zero ? z : rg.get_any<T>();

        eagine::simd::data_t<T, N, V> v{eagine::simd::fill<T, N, V>::apply(a)};

        const bool is_zero = eagine::simd::is_zero<T, N, V>::apply(v);

        if constexpr(std::is_integral_v<T>) {
            test.check_equal(a == z, is_zero, "is zero int");
        } else {
            test.check_equal(not(a > z or a < z), is_zero, "is zero float");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void simd_is_zero_TV(eagitest::case_& test) {
    simd_is_zero_TNV<T, 2, V>(test);
    simd_is_zero_TNV<T, 3, V>(test);
    simd_is_zero_TNV<T, 4, V>(test);
    simd_is_zero_TNV<T, 5, V>(test);
    simd_is_zero_TNV<T, 6, V>(test);
    simd_is_zero_TNV<T, 7, V>(test);
    simd_is_zero_TNV<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void simd_is_zero_T(eagitest::case_& test) {
    simd_is_zero_TV<T, true>(test);
    simd_is_zero_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void simd_is_zero_byte(auto& s) {
    eagitest::case_ test{s, 5, "byte"};
    simd_is_zero_T<unsigned char>(test);
}
//------------------------------------------------------------------------------
void simd_is_zero_int(auto& s) {
    eagitest::case_ test{s, 6, "int"};
    simd_is_zero_T<int>(test);
}
//------------------------------------------------------------------------------
void simd_is_zero_float(auto& s) {
    eagitest::case_ test{s, 7, "float"};
    simd_is_zero_T<float>(test);
}
//------------------------------------------------------------------------------
void simd_is_zero_double(auto& s) {
    eagitest::case_ test{s, 8, "double"};
    simd_is_zero_T<double>(test);
}
//------------------------------------------------------------------------------
// vector compare
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void simd_compare_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        const bool gen_equal = rg.get_int(0, 9) == 0;
        const T a = rg.get_any<T>();
        const T b = gen_equal ? a : rg.get_any<T>();

        eagine::simd::data_t<T, N, V> va{eagine::simd::fill<T, N, V>::apply(a)};
        eagine::simd::data_t<T, N, V> vb{eagine::simd::fill<T, N, V>::apply(b)};

        const auto cmp = eagine::simd::vector_compare<T, N, V>::apply(va, vb);

        test.check((a <=> b) == cmp, "ok");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void simd_compare_TV(eagitest::case_& test) {
    simd_compare_TNV<T, 2, V>(test);
    simd_compare_TNV<T, 3, V>(test);
    simd_compare_TNV<T, 4, V>(test);
    simd_compare_TNV<T, 5, V>(test);
    simd_compare_TNV<T, 6, V>(test);
    simd_compare_TNV<T, 7, V>(test);
    simd_compare_TNV<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void simd_compare_T(eagitest::case_& test) {
    simd_compare_TV<T, true>(test);
    simd_compare_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void simd_compare_byte(auto& s) {
    eagitest::case_ test{s, 9, "byte"};
    simd_compare_T<unsigned char>(test);
}
//------------------------------------------------------------------------------
void simd_compare_int(auto& s) {
    eagitest::case_ test{s, 10, "int"};
    simd_compare_T<int>(test);
}
//------------------------------------------------------------------------------
void simd_compare_float(auto& s) {
    eagitest::case_ test{s, 11, "float"};
    simd_compare_T<float>(test);
}
//------------------------------------------------------------------------------
void simd_compare_double(auto& s) {
    eagitest::case_ test{s, 12, "double"};
    simd_compare_T<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "simd_compare", 12};
    test.once(simd_fill_byte);
    test.once(simd_fill_int);
    test.once(simd_fill_float);
    test.once(simd_fill_double);
    test.once(simd_is_zero_byte);
    test.once(simd_is_zero_int);
    test.once(simd_is_zero_float);
    test.once(simd_is_zero_double);
    test.once(simd_compare_byte);
    test.once(simd_compare_int);
    test.once(simd_compare_float);
    test.once(simd_compare_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
