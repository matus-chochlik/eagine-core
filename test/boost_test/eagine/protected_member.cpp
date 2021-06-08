/*
 *  Copyright Matus Chochlik.
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */
#include <eagine/protected_member.hpp>
#include <eagine/selector.hpp>
#define BOOST_TEST_MODULE EAGINE_protected_member
#include "../unit_test_begin.inl"

#include <utility>

BOOST_AUTO_TEST_SUITE(protected_member_tests)

static eagine::test_random_generator rg;

template <typename T1, typename T2>
class eagine_protected_member_cls_1
  : eagine::protected_member<std::pair<T1, T2>> {
private:
    using selector = eagine::type_identity<std::pair<T1, T2>>;

public:
    eagine_protected_member_cls_1(T1 v1, T2 v2) {
        this->get_the_member() = std::make_pair(v1, v2);
    }

    T1 get_v1() {
        return eagine::get_member<selector>(*this).first;
    }

    T2 get_v2() const {
        return eagine::get_member<selector>(*this).second;
    }
};

template <typename T1, typename T2>
void eagine_test_protected_member_1() {
    for(int i = 0; i < 100; ++i) {
        T1 v1 = rg.get_any<T1>();
        T2 v2 = rg.get_any<T2>();

        eagine_protected_member_cls_1<T1, T2> x(v1, v2);

        BOOST_CHECK_EQUAL(x.get_v1(), v1);
        BOOST_CHECK_EQUAL(x.get_v2(), v2);
    }
}

BOOST_AUTO_TEST_CASE(protected_member_1) {
    using namespace eagine;

    eagine_test_protected_member_1<int, float>();
    eagine_test_protected_member_1<char, double>();
}

template <typename T1, typename T2, typename T3>
class eagine_protected_member_cls_2
  : eagine::protected_member<T1, eagine::selector<1>>
  , eagine::protected_member<T2, eagine::selector<2>>
  , eagine::protected_member<T3, eagine::selector<3>> {
public:
    eagine_protected_member_cls_2(T1 v1, T2 v2, T3 v3) {
        using namespace eagine;
        get_member<selector<1>>(*this) = v1;
        get_member<selector<2>>(*this) = v2;
        get_member<selector<3>>(*this) = v3;
    }

    T1 get_v1() {
        using namespace eagine;
        return get_member<selector<1>>(*this);
    }

    T2 get_v2() const {
        using namespace eagine;
        return get_member<selector<2>>(*this);
    }

    T3 get_v3() const {
        using namespace eagine;
        return get_member<selector<3>>(*this);
    }
};

template <typename T1, typename T2, typename T3>
void eagine_test_protected_member_2() {
    for(int i = 0; i < 100; ++i) {
        T1 v1 = rg.get_any<T1>();
        T2 v2 = rg.get_any<T2>();
        T3 v3 = rg.get_any<T3>();

        eagine_protected_member_cls_2<T1, T2, T3> x(v1, v2, v3);

        BOOST_CHECK_EQUAL(x.get_v1(), v1);
        BOOST_CHECK_EQUAL(x.get_v2(), v2);
        BOOST_CHECK_EQUAL(x.get_v3(), v3);
    }
}

BOOST_AUTO_TEST_CASE(protected_member_2) {
    using namespace eagine;

    eagine_test_protected_member_2<short, float, bool>();
    eagine_test_protected_member_2<char, double, char>();
    eagine_test_protected_member_2<int, int, int>();
}
// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
