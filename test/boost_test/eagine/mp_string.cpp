/*
 *  Copyright Matus Chochlik.
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */
#include <eagine/mp_string.hpp>
#define BOOST_TEST_MODULE EAGINE_mp_string_test
#include "../unit_test_begin.inl"

#include <cstring>

BOOST_AUTO_TEST_SUITE(mp_string_tests)

BOOST_AUTO_TEST_CASE(mp_string_empty) {
    using namespace eagine;

    using s = mp_string<>;
    BOOST_CHECK_EQUAL(std::strlen(s::value), 0);
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, 0);
    BOOST_CHECK(std::strcmp(s::value, "") == 0);
}

BOOST_AUTO_TEST_CASE(mp_string_chars) {
    using namespace eagine;

    using s = mp_string<'m', 'p', '_', 's', 't', 'r', 'i', 'n', 'g'>;
    BOOST_CHECK_EQUAL(std::strlen(s::value), std::strlen("mp_string"));
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, std::strlen("mp_string"));
    BOOST_CHECK(std::strcmp(s::value, "mp_string") == 0);
}

struct mp_blah {
    static constexpr const char mp_str[] = "blah";
};

BOOST_AUTO_TEST_CASE(mp_string_make) {
    using namespace eagine;

    using s = mp_make_string_t<mp_blah>;
    BOOST_CHECK_EQUAL(std::strlen(s::value), std::strlen("blah"));
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, std::strlen("blah"));
    BOOST_CHECK(std::strcmp(s::value, "blah") == 0);
}

BOOST_AUTO_TEST_CASE(mp_string_concat) {
    using namespace eagine;

    using foo = mp_string<'f', 'o', 'o'>;
    using bar = mp_string<'b', 'a', 'r'>;
    using baz = mp_string<'b', 'a', 'z'>;

    using s = mp_concat_t<foo, bar, baz>;

    BOOST_CHECK_EQUAL(std::strlen(s::value), std::strlen("foobarbaz"));
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, std::strlen("foobarbaz"));
    BOOST_CHECK(std::strcmp(s::value, "foobarbaz") == 0);
}

template <char C>
struct test_lshift : eagine::mp_string<C - 1> {};

BOOST_AUTO_TEST_CASE(mp_string_translate) {
    using namespace eagine;

    using bcdefg = mp_string<'b', 'c', 'd', 'e', 'f', 'g'>;

    using s = mp_translate_t<bcdefg, test_lshift>;

    BOOST_CHECK_EQUAL(std::strlen(s::value), std::strlen("abcdef"));
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, std::strlen("abcdef"));
    BOOST_CHECK(std::strcmp(s::value, "abcdef") == 0);
}

BOOST_AUTO_TEST_CASE(mp_int_to_string) {
    using namespace eagine;

    using s = mp_int_to_string_t<-1234567890>;

    BOOST_CHECK_EQUAL(std::strlen(s::value), 11);
    BOOST_CHECK_EQUAL(mp_strlen<s>::value, 11);
    BOOST_CHECK(std::strcmp(s::value, "-1234567890") == 0);
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
