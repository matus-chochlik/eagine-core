/*
 *  Copyright Matus Chochlik.
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */
#include <eagine/base64.hpp>
#define BOOST_TEST_MODULE EAGINE_biteset
#include "../unit_test_begin.inl"

#include <eagine/memory/span_algo.hpp>
#include <eagine/span.hpp>

BOOST_AUTO_TEST_SUITE(base64_tests)

static eagine::test_random_generator rg;

BOOST_AUTO_TEST_CASE(base64_trip) {
    using namespace eagine;

    std::vector<byte> orig;
    std::vector<byte> deco;
    std::string enco;

    for(int i = 0; i < test_repeats(1000, 10000); ++i) {

        orig.resize(rg.get<std::size_t>(0, 10000));
        rg.fill(orig);

        auto res = bool(base64_encode(view(orig), enco));
        BOOST_CHECK(res);
        if(res) {
            res = bool(base64_decode(view(enco), deco));
            BOOST_CHECK(res);
            if(res) {
                BOOST_CHECK(orig == deco);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
