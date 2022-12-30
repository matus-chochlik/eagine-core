/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
//------------------------------------------------------------------------------
// area
//------------------------------------------------------------------------------
void dimension_area(auto& s) {
    eagitest::case_ test{s, 1, "area"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::area()) ==
            get_pow_of_dim(bd, eagine::units::length()) +
              get_pow_of_dim(bd, eagine::units::length()),
          "1");
    });
}
//------------------------------------------------------------------------------
// volume
//------------------------------------------------------------------------------
void dimension_volume(auto& s) {
    eagitest::case_ test{s, 2, "volume"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::volume()) ==
            get_pow_of_dim(bd, eagine::units::length()) +
              get_pow_of_dim(bd, eagine::units::length()) +
              get_pow_of_dim(bd, eagine::units::length()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::volume()) ==
            get_pow_of_dim(bd, eagine::units::area()) +
              get_pow_of_dim(bd, eagine::units::length()),
          "2");
    });
}
//------------------------------------------------------------------------------
// mass_density
//------------------------------------------------------------------------------
void dimension_mass_density(auto& s) {
    eagitest::case_ test{s, 3, "mass_density"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::mass_density()) ==
            get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::volume()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::mass_density()) ==
            get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::length()),
          "2");
    });
}
//------------------------------------------------------------------------------
// velocity
//------------------------------------------------------------------------------
void dimension_velocity(auto& s) {
    eagitest::case_ test{s, 4, "velocity"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::velocity()) ==
            get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");
    });
}
//------------------------------------------------------------------------------
// angular velocity
//------------------------------------------------------------------------------
void dimension_angular_velocity(auto& s) {
    eagitest::case_ test{s, 5, "angular velocity"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::angular_velocity()) ==
            get_pow_of_dim(bd, eagine::units::angle()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");
    });
}
//------------------------------------------------------------------------------
// acceleration
//------------------------------------------------------------------------------
void dimension_acceleration(auto& s) {
    eagitest::case_ test{s, 6, "acceleration"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::acceleration()) ==
            get_pow_of_dim(bd, eagine::units::velocity()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::acceleration()) ==
            get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::time()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "2");
    });
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "dimension", 6};
    test.once(dimension_area);
    test.once(dimension_volume);
    test.once(dimension_mass_density);
    test.once(dimension_velocity);
    test.once(dimension_angular_velocity);
    test.once(dimension_acceleration);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
