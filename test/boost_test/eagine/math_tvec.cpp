/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/math/tvec.hpp>
#define BOOST_TEST_MODULE EAGINE_math_tvec
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(math_tvec_tests)

static eagine::test_random_generator rg;

BOOST_AUTO_TEST_CASE(math_tvec_vec_val_ctr) {
    test_math_tvec_vec_val_ctr_T<int>();
    test_math_tvec_vec_val_ctr_T<float>();
    test_math_tvec_vec_val_ctr_T<double>();
}

template <typename T, bool V>
void test_math_tvec_vec_vec_ctr_TV() {
    T ra = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> va(ra);
    BOOST_CHECK_EQUAL(va[0], ra);

    T rb = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> vb(rb);
    BOOST_CHECK_EQUAL(vb[0], rb);

    eagine::math::tvec<T, 2, V> vaa(va, va);
    BOOST_CHECK_EQUAL(vaa[0], ra);
    BOOST_CHECK_EQUAL(vaa[1], ra);

    eagine::math::tvec<T, 2, V> vab(va, vb);
    BOOST_CHECK_EQUAL(vab[0], ra);
    BOOST_CHECK_EQUAL(vab[1], rb);

    eagine::math::tvec<T, 2, V> vbb(vb, vb);
    BOOST_CHECK_EQUAL(vbb[0], rb);
    BOOST_CHECK_EQUAL(vbb[1], rb);

    eagine::math::tvec<T, 3, V> vaba(vab, va);
    BOOST_CHECK_EQUAL(vaba[0], ra);
    BOOST_CHECK_EQUAL(vaba[1], rb);
    BOOST_CHECK_EQUAL(vaba[2], ra);

    eagine::math::tvec<T, 3, V> vbab(vb, vab);
    BOOST_CHECK_EQUAL(vbab[0], rb);
    BOOST_CHECK_EQUAL(vbab[1], ra);
    BOOST_CHECK_EQUAL(vbab[2], rb);

    eagine::math::tvec<T, 4, V> vabab(vaba, vb);
    BOOST_CHECK_EQUAL(vabab[0], ra);
    BOOST_CHECK_EQUAL(vabab[1], rb);
    BOOST_CHECK_EQUAL(vabab[2], ra);
    BOOST_CHECK_EQUAL(vabab[3], rb);

    eagine::math::tvec<T, 4, V> vaabb(vaa, vbb);
    BOOST_CHECK_EQUAL(vaabb[0], ra);
    BOOST_CHECK_EQUAL(vaabb[1], ra);
    BOOST_CHECK_EQUAL(vaabb[2], rb);
    BOOST_CHECK_EQUAL(vaabb[3], rb);

    eagine::math::tvec<T, 5, V> vababa(vabab, va);
    BOOST_CHECK_EQUAL(vababa[0], ra);
    BOOST_CHECK_EQUAL(vababa[1], rb);
    BOOST_CHECK_EQUAL(vababa[2], ra);
    BOOST_CHECK_EQUAL(vababa[3], rb);
    BOOST_CHECK_EQUAL(vababa[4], ra);

    eagine::math::tvec<T, 5, V> vababb(vaba, vbb);
    BOOST_CHECK_EQUAL(vababb[0], ra);
    BOOST_CHECK_EQUAL(vababb[1], rb);
    BOOST_CHECK_EQUAL(vababb[2], ra);
    BOOST_CHECK_EQUAL(vababb[3], rb);
    BOOST_CHECK_EQUAL(vababb[4], rb);

    eagine::math::tvec<T, 5, V> vaabab(vaa, vbab);
    BOOST_CHECK_EQUAL(vaabab[0], ra);
    BOOST_CHECK_EQUAL(vaabab[1], ra);
    BOOST_CHECK_EQUAL(vaabab[2], rb);
    BOOST_CHECK_EQUAL(vaabab[3], ra);
    BOOST_CHECK_EQUAL(vaabab[4], rb);

    eagine::math::tvec<T, 5, V> vbabab(vb, vabab);
    BOOST_CHECK_EQUAL(vbabab[0], rb);
    BOOST_CHECK_EQUAL(vbabab[1], ra);
    BOOST_CHECK_EQUAL(vbabab[2], rb);
    BOOST_CHECK_EQUAL(vbabab[3], ra);
    BOOST_CHECK_EQUAL(vbabab[4], rb);
}

template <typename T>
void test_math_tvec_vec_vec_ctr_T() {
    test_math_tvec_vec_vec_ctr_TV<T, true>();
    test_math_tvec_vec_vec_ctr_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(math_tvec_vec_vec_ctr) {
    test_math_tvec_vec_vec_ctr_T<int>();
    test_math_tvec_vec_vec_ctr_T<float>();
    test_math_tvec_vec_vec_ctr_T<double>();
}

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
