/*
 *  Copyright Matus Chochlik.
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */
#include <eagine/make_array.hpp>
#define BOOST_TEST_MODULE EAGINE_make_array
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(make_array_tests)

BOOST_AUTO_TEST_CASE(make_array_1) {
    using namespace eagine;

    std::array<int, 10> ai1 = make_array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    EAGINE_MAYBE_UNUSED(ai1);

    std::array<float, 5> af1 = make_array(0.f, 2.f, 4.f, 6.f, 8.f);
    EAGINE_MAYBE_UNUSED(af1);
}

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
