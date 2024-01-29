/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.vectorization;
//------------------------------------------------------------------------------
// default construction
//------------------------------------------------------------------------------
template <typename T, bool V, typename Suite>
void vect_data_default_construct_TV(Suite& s) {
    eagitest::case_ test{s, 1, "default construct"};
    typename eagine::vect::data<T, 1, V>::type v1{};
    test.constructed(v1, "v1");
    typename eagine::vect::data<T, 2, V>::type v2{};
    test.constructed(v2, "v2");
    typename eagine::vect::data<T, 3, V>::type v3{};
    test.constructed(v3, "v3");
    typename eagine::vect::data<T, 4, V>::type v4{};
    test.constructed(v4, "v4");
    typename eagine::vect::data<T, 5, V>::type v5{};
    test.constructed(v5, "v5");
    typename eagine::vect::data<T, 6, V>::type v6{};
    test.constructed(v6, "v6");
    typename eagine::vect::data<T, 7, V>::type v7{};
    test.constructed(v7, "v7");
    typename eagine::vect::data<T, 8, V>::type v8{};
    test.constructed(v8, "v8");
}
//------------------------------------------------------------------------------
template <typename T, typename Suite>
void vect_data_default_construct_T(Suite& s) {
    vect_data_default_construct_TV<T, true>(s);
    vect_data_default_construct_TV<T, false>(s);
}
//------------------------------------------------------------------------------
void vect_data_default_construct(auto& s) {
    vect_data_default_construct_T<int>(s);
    vect_data_default_construct_T<float>(s);
    vect_data_default_construct_T<double>(s);
}
//------------------------------------------------------------------------------
// initialization
//------------------------------------------------------------------------------
template <typename T, bool V, typename Suite>
void vect_data_initialization_TV(Suite& s) {
    eagitest::case_ test{s, 2, "initialize"};

    typename eagine::vect::data<T, 1, V>::type v1 = {T(1)};
    test.constructed(v1, "v1");
    typename eagine::vect::data<T, 2, V>::type v2 = {T(1), T(2)};
    test.constructed(v2, "v2");
    typename eagine::vect::data<T, 3, V>::type v3 = {T(1), T(2), T(3)};
    test.constructed(v3, "v3");
    typename eagine::vect::data<T, 4, V>::type v4 = {T(1), T(2), T(3), T(4)};
    test.constructed(v4, "v4");
    typename eagine::vect::data<T, 5, V>::type v5 = {
      T(1), T(2), T(3), T(4), T(5)};
    test.constructed(v5, "v5");
    typename eagine::vect::data<T, 6, V>::type v6 = {
      T(1), T(2), T(3), T(4), T(5), T(6)};
    test.constructed(v6, "v6");
    typename eagine::vect::data<T, 7, V>::type v7 = {
      T(1), T(2), T(3), T(4), T(5), T(6), T(7)};
    test.constructed(v7, "v7");
    typename eagine::vect::data<T, 8, V>::type v8 = {
      T(1), T(2), T(3), T(4), T(5), T(6), T(7), T(8)};
    test.constructed(v8, "v8");
}
//------------------------------------------------------------------------------
template <typename T, typename Suite>
void vect_data_initialization_T(Suite& s) {
    vect_data_initialization_TV<T, true>(s);
    vect_data_initialization_TV<T, false>(s);
}
//------------------------------------------------------------------------------
void vect_data_initialization(auto& s) {
    vect_data_initialization_T<int>(s);
    vect_data_initialization_T<float>(s);
    vect_data_initialization_T<double>(s);
}
//------------------------------------------------------------------------------
// copy construction
//------------------------------------------------------------------------------
template <typename T, bool V1, bool V2, typename Suite>
void vect_data_copy_construction_TV(Suite& s) {
    eagitest::case_ test{s, 3, "copy construct"};

    typename eagine::vect::data<T, 1, V1>::type v1{};
    typename eagine::vect::data<T, 1, V2>::type v1c(v1);
    test.constructed(v1c, "v1c");
    typename eagine::vect::data<T, 2, V1>::type v2{};
    typename eagine::vect::data<T, 2, V2>::type v2c(v2);
    test.constructed(v2c, "v2c");
    typename eagine::vect::data<T, 3, V1>::type v3{};
    typename eagine::vect::data<T, 3, V2>::type v3c(v3);
    test.constructed(v3c, "v3c");
    typename eagine::vect::data<T, 4, V1>::type v4{};
    typename eagine::vect::data<T, 4, V2>::type v4c(v4);
    test.constructed(v4c, "v4c");
    typename eagine::vect::data<T, 5, V1>::type v5{};
    typename eagine::vect::data<T, 5, V2>::type v5c(v5);
    test.constructed(v5c, "v5c");
    typename eagine::vect::data<T, 6, V1>::type v6{};
    typename eagine::vect::data<T, 6, V2>::type v6c(v6);
    test.constructed(v6c, "v6c");
    typename eagine::vect::data<T, 7, V1>::type v7{};
    typename eagine::vect::data<T, 7, V2>::type v7c(v7);
    test.constructed(v7c, "v7c");
    typename eagine::vect::data<T, 8, V1>::type v8{};
    typename eagine::vect::data<T, 8, V2>::type v8c(v8);
    test.constructed(v8c, "v8c");
}
//------------------------------------------------------------------------------
template <typename T, typename Suite>
void vect_data_copy_construction_T(Suite& s) {
    vect_data_copy_construction_TV<T, true, true>(s);
    // TODO
    //  vect_data_copy_construction_TV<T, true, false>(s);
    //  vect_data_copy_construction_TV<T, false, true>(s);
    vect_data_copy_construction_TV<T, false, false>(s);
}
//------------------------------------------------------------------------------
void vect_data_copy_construction(auto& s) {
    vect_data_copy_construction_T<int>(s);
    vect_data_copy_construction_T<float>(s);
    vect_data_copy_construction_T<double>(s);
}
//------------------------------------------------------------------------------
// copy assignment
//------------------------------------------------------------------------------
template <typename T, bool V1, bool V2, typename Suite>
void vect_data_copy_assignment_TV(Suite& s) {
    eagitest::case_ test{s, 4, "copy assign"};

    typename eagine::vect::data<T, 1, V1>::type v1{};
    typename eagine::vect::data<T, 1, V2>::type v1c = v1;
    test.constructed(v1c, "v1c");
    typename eagine::vect::data<T, 2, V1>::type v2{};
    typename eagine::vect::data<T, 2, V2>::type v2c = v2;
    test.constructed(v2c, "v2c");
    typename eagine::vect::data<T, 3, V1>::type v3{};
    typename eagine::vect::data<T, 3, V2>::type v3c = v3;
    test.constructed(v3c, "v3c");
    typename eagine::vect::data<T, 4, V1>::type v4{};
    typename eagine::vect::data<T, 4, V2>::type v4c = v4;
    test.constructed(v4c, "v4c");
    typename eagine::vect::data<T, 5, V1>::type v5{};
    typename eagine::vect::data<T, 5, V2>::type v5c = v5;
    test.constructed(v5c, "v5c");
    typename eagine::vect::data<T, 6, V1>::type v6{};
    typename eagine::vect::data<T, 6, V2>::type v6c = v6;
    test.constructed(v6c, "v6c");
    typename eagine::vect::data<T, 7, V1>::type v7{};
    typename eagine::vect::data<T, 7, V2>::type v7c = v7;
    test.constructed(v7c, "v7c");
    typename eagine::vect::data<T, 8, V1>::type v8{};
    typename eagine::vect::data<T, 8, V2>::type v8c = v8;
    test.constructed(v8c, "v8c");
}
//------------------------------------------------------------------------------
template <typename T, typename Suite>
void vect_data_copy_assignment_T(Suite& s) {
    vect_data_copy_assignment_TV<T, true, true>(s);
    // TODO
    // vect_data_copy_assignment_TV<T, true, false>(s);
    //  vect_data_copy_assignment_TV<T, false, true>(s);
    vect_data_copy_assignment_TV<T, false, false>(s);
}
//------------------------------------------------------------------------------
void vect_data_copy_assignment(auto& s) {
    vect_data_copy_assignment_T<int>(s);
    vect_data_copy_assignment_T<float>(s);
    vect_data_copy_assignment_T<double>(s);
}
//------------------------------------------------------------------------------
// elements
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_data_elements_TV(eagitest::case_& test) {
    typename eagine::vect::data<T, 1, V>::type v1{T(1)};
    test.check_equal(v1[0], T(1), "1");

    typename eagine::vect::data<T, 2, V>::type v2 = {T(1), T(2)};
    test.check_equal(v2[0], T(1), "1");
    test.check_equal(v2[1], T(2), "2");

    typename eagine::vect::data<T, 3, V>::type v3{T(2), T(3), T(4)};
    test.check_equal(v3[0], T(2), "2");
    test.check_equal(v3[1], T(3), "3");
    test.check_equal(v3[2], T(4), "4");

    typename eagine::vect::data<T, 4, V>::type v4 = {T(3), T(4), T(5), T(6)};
    test.check_equal(v4[0], T(3), "3");
    test.check_equal(v4[1], T(4), "4");
    test.check_equal(v4[2], T(5), "5");
    test.check_equal(v4[3], T(6), "6");

    typename eagine::vect::data<T, 7, V>::type v7 = {
      T(4), T(5), T(6), T(7), T(8), T(9), T(0)};
    test.check_equal(v7[0], T(4), "4");
    test.check_equal(v7[1], T(5), "5");
    test.check_equal(v7[2], T(6), "6");
    test.check_equal(v7[3], T(7), "7");
    test.check_equal(v7[4], T(8), "8");
    test.check_equal(v7[5], T(9), "9");
    test.check_equal(v7[6], T(0), "0");

    typename eagine::vect::data<T, 8, V>::type v8{
      T(5), T(6), T(7), T(8), T(9), T(0), T(1), T(2)};
    test.check_equal(v8[0], T(5), "5");
    test.check_equal(v8[1], T(6), "6");
    test.check_equal(v8[2], T(7), "7");
    test.check_equal(v8[3], T(8), "8");
    test.check_equal(v8[4], T(9), "9");
    test.check_equal(v8[5], T(0), "0");
    test.check_equal(v8[6], T(1), "1");
    test.check_equal(v8[7], T(2), "2");
}
//------------------------------------------------------------------------------
template <typename T>
void vect_data_elements_T(eagitest::case_& test) {
    vect_data_elements_TV<T, true>(test);
    vect_data_elements_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_data_elements_int(auto& s) {
    eagitest::case_ test{s, 5, "element access int"};
    vect_data_elements_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_data_elements_float(auto& s) {
    eagitest::case_ test{s, 6, "element access float"};
    vect_data_elements_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_data_elements_double(auto& s) {
    eagitest::case_ test{s, 7, "element access double"};
    vect_data_elements_T<double>(test);
}
//------------------------------------------------------------------------------
// addition
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_data_add_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N], b[N];
        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_between<T>(-1000, 1000);
            b[i] = rg.get_between<T>(-1000, 1000);
        }

        typename eagine::vect::data<T, N, V>::type vNa = {};
        typename eagine::vect::data<T, N, V>::type vNb = {};

        for(int i = 0; i < N; ++i) {
            vNa[i] = a[i];
            vNb[i] = b[i];
        }

        typename eagine::vect::data<T, N, V>::type vNc = vNa + vNb;
        typename eagine::vect::data<T, N, V>::type vNd = vNb + vNa;

        for(int i = 0; i < N; ++i) {
            test.check_close<T>(vNc[i], vNd[i], "close 0");
            test.check_close<T>(vNc[i], a[i] + b[i], "close 1");
            test.check_close<T>(vNd[i], b[i] + a[i], "close 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, int N>
void vect_data_add_TV(eagitest::case_& test) {
    vect_data_add_TNV<T, N, true>(test);
    vect_data_add_TNV<T, N, false>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_data_add_T(eagitest::case_& test) {
    vect_data_add_TV<T, 1>(test);
    vect_data_add_TV<T, 2>(test);
    vect_data_add_TV<T, 3>(test);
    vect_data_add_TV<T, 4>(test);
    vect_data_add_TV<T, 5>(test);
    vect_data_add_TV<T, 6>(test);
    vect_data_add_TV<T, 7>(test);
    vect_data_add_TV<T, 8>(test);
}
//------------------------------------------------------------------------------
void vect_data_add_int(auto& s) {
    eagitest::case_ test{s, 8, "int addition"};
    vect_data_add_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_data_add_float(auto& s) {
    eagitest::case_ test{s, 9, "float addition"};
    vect_data_add_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_data_add_double(auto& s) {
    eagitest::case_ test{s, 10, "double addition"};
    vect_data_add_T<double>(test);
}
//------------------------------------------------------------------------------
// subtraction
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_data_subtract_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N], b[N];
        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_between<T>(-1000, 1000);
            b[i] = rg.get_between<T>(-1000, 1000);
        }

        typename eagine::vect::data<T, N, V>::type vNa = {};
        typename eagine::vect::data<T, N, V>::type vNb = {};

        for(int i = 0; i < N; ++i) {
            vNa[i] = a[i];
            vNb[i] = b[i];
        }

        typename eagine::vect::data<T, N, V>::type vNc = vNa - vNb;
        typename eagine::vect::data<T, N, V>::type vNd = vNb - vNa;

        for(int i = 0; i < N; ++i) {
            test.check_close<T>(vNc[i], a[i] - b[i], "compare 1");
            test.check_close<T>(vNd[i], b[i] - a[i], "compare 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, int N>
void vect_data_subtract_TV(eagitest::case_& test) {
    vect_data_subtract_TNV<T, N, true>(test);
    vect_data_subtract_TNV<T, N, false>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_data_subtract_T(eagitest::case_& test) {
    vect_data_subtract_TV<T, 1>(test);
    vect_data_subtract_TV<T, 2>(test);
    vect_data_subtract_TV<T, 3>(test);
    vect_data_subtract_TV<T, 4>(test);
    vect_data_subtract_TV<T, 5>(test);
    vect_data_subtract_TV<T, 6>(test);
    vect_data_subtract_TV<T, 7>(test);
    vect_data_subtract_TV<T, 8>(test);
}
//------------------------------------------------------------------------------
void vect_data_subtract_int(auto& s) {
    eagitest::case_ test{s, 11, "int subtraction"};
    vect_data_subtract_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_data_subtract_float(auto& s) {
    eagitest::case_ test{s, 12, "float subtraction"};
    vect_data_subtract_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_data_subtract_double(auto& s) {
    eagitest::case_ test{s, 13, "double subtraction"};
    vect_data_subtract_T<double>(test);
}
//------------------------------------------------------------------------------
// multiplication
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_data_multiply_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N], b[N];
        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_between<T>(-1000, 1000);
            b[i] = rg.get_between<T>(-1000, 1000);
        }

        typename eagine::vect::data<T, N, V>::type vNa = {};
        typename eagine::vect::data<T, N, V>::type vNb = {};

        for(int i = 0; i < N; ++i) {
            vNa[i] = a[i];
            vNb[i] = b[i];
        }

        typename eagine::vect::data<T, N, V>::type vNc = vNa * vNb;
        typename eagine::vect::data<T, N, V>::type vNd = vNb * vNa;

        for(int i = 0; i < N; ++i) {
            test.check_close<T>(vNc[i], vNd[i], "close 0");
            test.check_close<T>(vNc[i], a[i] * b[i], "close 1");
            test.check_close<T>(vNd[i], b[i] * a[i], "close 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, int N>
void vect_data_multiply_TV(eagitest::case_& test) {
    vect_data_multiply_TNV<T, N, true>(test);
    vect_data_multiply_TNV<T, N, false>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_data_multiply_T(eagitest::case_& test) {
    vect_data_multiply_TV<T, 1>(test);
    vect_data_multiply_TV<T, 2>(test);
    vect_data_multiply_TV<T, 3>(test);
    vect_data_multiply_TV<T, 4>(test);
    vect_data_multiply_TV<T, 5>(test);
    vect_data_multiply_TV<T, 6>(test);
    vect_data_multiply_TV<T, 7>(test);
    vect_data_multiply_TV<T, 8>(test);
}
//------------------------------------------------------------------------------
void vect_data_multiply_int(auto& s) {
    eagitest::case_ test{s, 14, "int multiplication"};
    vect_data_multiply_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_data_multiply_float(auto& s) {
    eagitest::case_ test{s, 15, "float multiplication"};
    vect_data_multiply_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_data_multiply_double(auto& s) {
    eagitest::case_ test{s, 16, "double multiplication"};
    vect_data_multiply_T<double>(test);
}
//------------------------------------------------------------------------------
// division
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_data_divide_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N], b[N];
        for(int i = 0; i < N; ++i) {
            do {
                a[i] = rg.get_between<T>(-1000, 1000);
            } while(not((a[i] > T(0)) or (a[i] < T(0))));
            do {
                b[i] = rg.get_between<T>(-1000, 1000);
            } while(not((b[i] > T(0)) or (b[i] < T(0))));
        }

        typename eagine::vect::data<T, N, V>::type vNa = {};
        typename eagine::vect::data<T, N, V>::type vNb = {};

        for(int i = 0; i < N; ++i) {
            vNa[i] = a[i];
            vNb[i] = b[i];
        }

        typename eagine::vect::data<T, N, V>::type vNc =
          eagine::vect::sdiv<T, N, V>::apply(vNa, vNb);
        typename eagine::vect::data<T, N, V>::type vNd =
          eagine::vect::sdiv<T, N, V>::apply(vNb, vNa);

        for(int i = 0; i < N; ++i) {
            test.check_close<T>(vNc[i], a[i] / b[i], "compare 1");
            test.check_close<T>(vNd[i], b[i] / a[i], "compare 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, int N>
void vect_data_divide_TV(eagitest::case_& test) {
    vect_data_divide_TNV<T, N, true>(test);
    vect_data_divide_TNV<T, N, false>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_data_divide_T(eagitest::case_& test) {
    vect_data_divide_TV<T, 1>(test);
    vect_data_divide_TV<T, 2>(test);
    vect_data_divide_TV<T, 3>(test);
    vect_data_divide_TV<T, 4>(test);
    vect_data_divide_TV<T, 5>(test);
    vect_data_divide_TV<T, 6>(test);
    vect_data_divide_TV<T, 7>(test);
    vect_data_divide_TV<T, 8>(test);
}
//------------------------------------------------------------------------------
void vect_data_divide_int(auto& s) {
    eagitest::case_ test{s, 17, "division"};
    vect_data_divide_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_data_divide_float(auto& s) {
    eagitest::case_ test{s, 18, "division"};
    vect_data_divide_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_data_divide_double(auto& s) {
    eagitest::case_ test{s, 19, "division"};
    vect_data_divide_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_data", 19};
    test.once(vect_data_default_construct);
    test.once(vect_data_initialization);
    test.once(vect_data_copy_construction);
    test.once(vect_data_copy_assignment);
    test.once(vect_data_elements_int);
    test.once(vect_data_elements_float);
    test.once(vect_data_elements_double);
    test.once(vect_data_add_int);
    test.once(vect_data_add_float);
    test.once(vect_data_add_double);
    test.once(vect_data_subtract_int);
    test.once(vect_data_subtract_float);
    test.once(vect_data_subtract_double);
    test.once(vect_data_multiply_int);
    test.once(vect_data_multiply_float);
    test.once(vect_data_multiply_double);
    test.once(vect_data_divide_int);
    test.once(vect_data_divide_float);
    test.once(vect_data_divide_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
