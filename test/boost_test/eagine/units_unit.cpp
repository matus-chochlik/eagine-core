/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/units/dimension.hpp>
#include <eagine/units/unit.hpp>
#define BOOST_TEST_MODULE EAGINE_units_unit
#include "../unit_test_begin.inl"

namespace eagine {
namespace units {

struct mock_system {};

} // namespace units
} // namespace eagine

BOOST_AUTO_TEST_SUITE(units_unit_tests)

struct units_unit_add_tester {
    template <typename BD>
    void operator()(BD) const {
        using namespace eagine::units;

        using D = dimension<BD, 1>;

        unit<D, mock_system> a, b;
        [[maybe_unused]] unit<D, mock_system> c = a + b;
    }
};

BOOST_AUTO_TEST_CASE(units_unit_add) {
    using namespace eagine::units;

    base::for_each_dim(units_unit_add_tester());
}

struct units_unit_sub_tester {
    template <typename BD>
    void operator()(BD) const {
        using namespace eagine::units;

        using D = dimension<BD, 1>;

        unit<D, mock_system> a, b;
        [[maybe_unused]] unit<D, mock_system> c = a - b;
    }
};

BOOST_AUTO_TEST_CASE(units_unit_sub) {
    using namespace eagine::units;

    base::for_each_dim(units_unit_sub_tester());
}

template <typename BD1>
struct units_unit_mul_tester {
    template <typename BD2>
    void operator()(BD2) const {
        using namespace eagine::units;

        using D1 = dimension<BD1, 1>;
        using D2 = dimension<BD2, 1>;
        using D = decltype(D1() * D2());

        unit<D1, mock_system> a;
        unit<D2, mock_system> b;
        [[maybe_unused]] unit<D, mock_system> c = a * b;
    }
};

struct units_unit_mul_hlp_tester {
    template <typename BD>
    void operator()(BD) const {
        using namespace eagine::units;

        base::for_each_dim(units_unit_mul_tester<BD>());
    }
};

BOOST_AUTO_TEST_CASE(units_unit_mul) {
    using namespace eagine::units;

    base::for_each_dim(units_unit_mul_hlp_tester());
}

template <typename BD1>
struct units_unit_div_tester {
    template <typename BD2>
    void operator()(BD2) const {
        using namespace eagine::units;

        using D1 = dimension<BD1, 1>;
        using D2 = dimension<BD2, 1>;
        using D = decltype(D1() / D2());

        unit<D1, mock_system> a;
        unit<D2, mock_system> b;
        [[maybe_unused]] unit<D, mock_system> c = a / b;
    }
};

struct units_unit_div_hlp_tester {
    template <typename BD>
    void operator()(BD) const {
        using namespace eagine::units;

        base::for_each_dim(units_unit_div_tester<BD>());
    }
};

BOOST_AUTO_TEST_CASE(units_unit_div) {
    using namespace eagine::units;

    base::for_each_dim(units_unit_div_hlp_tester());
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
