/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
//------------------------------------------------------------------------------
namespace eagine {
namespace units {

struct mock_system {};

} // namespace units
} // namespace eagine
//------------------------------------------------------------------------------
// unit addition
//------------------------------------------------------------------------------
void unit_addition(auto& s) {
    eagitest::case_ test{s, 1, "addition"};
    using namespace eagine;

    eagine::units::base::for_each_dim([&](auto bd) {
        using namespace eagine::units;
        using D = dimension<decltype(bd), 1>;

        unit<D, mock_system> a, b;
        [[maybe_unused]] unit<D, mock_system> c = a + b;
    });
}
//------------------------------------------------------------------------------
// unit subtraction
//------------------------------------------------------------------------------
void unit_subtraction(auto& s) {
    eagitest::case_ test{s, 2, "subtraction"};
    using namespace eagine;

    eagine::units::base::for_each_dim([&](auto bd) {
        using namespace eagine::units;
        using D = dimension<decltype(bd), 1>;

        unit<D, mock_system> a, b;
        [[maybe_unused]] unit<D, mock_system> c = a - b;
    });
}
//------------------------------------------------------------------------------
// unit multiplication
//------------------------------------------------------------------------------
void unit_multiplication(auto& s) {
    eagitest::case_ test{s, 3, "multiplication"};
    using namespace eagine;

    eagine::units::base::for_each_dim([&](auto bdl) {
        using namespace eagine::units;
        using Dl = dimension<decltype(bdl), 1>;

        eagine::units::base::for_each_dim([&](auto bdr) {
            using Dr = dimension<decltype(bdr), 1>;
            using D = decltype(std::declval<Dl>() * std::declval<Dr>());
            unit<Dl, mock_system> a;
            unit<Dr, mock_system> b;
            [[maybe_unused]] unit<D, mock_system> c = a * b;
        });
    });
}
//------------------------------------------------------------------------------
// unit division
//------------------------------------------------------------------------------
void unit_division(auto& s) {
    eagitest::case_ test{s, 4, "division"};
    using namespace eagine;

    eagine::units::base::for_each_dim([&](auto bdl) {
        using namespace eagine::units;
        using Dl = dimension<decltype(bdl), 1>;

        eagine::units::base::for_each_dim([&](auto bdr) {
            using Dr = dimension<decltype(bdr), 1>;
            using D = decltype(std::declval<Dl>() / std::declval<Dr>());
            unit<Dl, mock_system> a;
            unit<Dr, mock_system> b;
            [[maybe_unused]] unit<D, mock_system> c = a / b;
        });
    });
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "unit", 4};
    test.once(unit_addition);
    test.once(unit_subtraction);
    test.once(unit_multiplication);
    test.once(unit_division);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

