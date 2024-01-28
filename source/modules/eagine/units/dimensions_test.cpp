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
// momentum
//------------------------------------------------------------------------------
void dimension_momentum(auto& s) {
    eagitest::case_ test{s, 7, "momentum"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::momentum()) ==
            get_pow_of_dim(bd, eagine::units::velocity()) +
              get_pow_of_dim(bd, eagine::units::mass()),
          "1");
    });
}
//------------------------------------------------------------------------------
// force
//------------------------------------------------------------------------------
void dimension_force(auto& s) {
    eagitest::case_ test{s, 8, "force"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::force()) ==
            get_pow_of_dim(bd, eagine::units::acceleration()) +
              get_pow_of_dim(bd, eagine::units::mass()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::force()) ==
            get_pow_of_dim(bd, eagine::units::velocity()) +
              get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "2");
    });
}
//------------------------------------------------------------------------------
// energy
//------------------------------------------------------------------------------
void dimension_energy(auto& s) {
    eagitest::case_ test{s, 9, "energy"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::energy()) ==
            get_pow_of_dim(bd, eagine::units::force()) +
              get_pow_of_dim(bd, eagine::units::length()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::energy()) ==
            get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::time()) +
              get_pow_of_dim(bd, eagine::units::length()) -
              get_pow_of_dim(bd, eagine::units::time()) +
              get_pow_of_dim(bd, eagine::units::mass()),
          "2");
    });
}
//------------------------------------------------------------------------------
// power
//------------------------------------------------------------------------------
void dimension_power(auto& s) {
    eagitest::case_ test{s, 10, "power"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::power()) ==
            get_pow_of_dim(bd, eagine::units::energy()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::power()) ==
            get_pow_of_dim(bd, eagine::units::area()) +
              get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::time()) * 3,
          "2");
    });
}
//------------------------------------------------------------------------------
// pressure
//------------------------------------------------------------------------------
void dimension_pressure(auto& s) {
    eagitest::case_ test{s, 11, "pressure"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::pressure()) ==
            get_pow_of_dim(bd, eagine::units::force()) -
              get_pow_of_dim(bd, eagine::units::area()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::pressure()) ==
            get_pow_of_dim(bd, eagine::units::velocity()) +
              get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::time()) -
              get_pow_of_dim(bd, eagine::units::area()),
          "2");
    });
}
//------------------------------------------------------------------------------
// electric_charge
//------------------------------------------------------------------------------
void dimension_electric_charge(auto& s) {
    eagitest::case_ test{s, 12, "electric charge"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::electric_charge()) ==
            get_pow_of_dim(bd, eagine::units::electric_current()) +
              get_pow_of_dim(bd, eagine::units::time()),
          "1");
    });
}
//------------------------------------------------------------------------------
// electric_tension
//------------------------------------------------------------------------------
void dimension_electric_tension(auto& s) {
    eagitest::case_ test{s, 13, "electric tension"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::electric_tension()) ==
            get_pow_of_dim(bd, eagine::units::power()) -
              get_pow_of_dim(bd, eagine::units::electric_current()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::electric_tension()) ==
            get_pow_of_dim(bd, eagine::units::length()) * 2 +
              get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::time()) * 3 -
              get_pow_of_dim(bd, eagine::units::electric_current()),
          "2");
    });
}
//------------------------------------------------------------------------------
// electric_capacitance
//------------------------------------------------------------------------------
void dimension_electric_capacitance(auto& s) {
    eagitest::case_ test{s, 14, "electric capacitance"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_capacitance()) ==
            get_pow_of_dim(bd, eagine::units::electric_charge()) -
              get_pow_of_dim(bd, eagine::units::electric_tension()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_capacitance()) ==
            get_pow_of_dim(bd, eagine::units::electric_current()) * 2 -
              get_pow_of_dim(bd, eagine::units::length()) * 2 -
              get_pow_of_dim(bd, eagine::units::mass()) +
              get_pow_of_dim(bd, eagine::units::time()) * 4,
          "2");
    });
}
//------------------------------------------------------------------------------
// electric_conductance
//------------------------------------------------------------------------------
void dimension_electric_conductance(auto& s) {
    eagitest::case_ test{s, 15, "electric conductance"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_conductance()) ==
            get_pow_of_dim(bd, eagine::units::electric_current()) -
              get_pow_of_dim(bd, eagine::units::electric_tension()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_conductance()) ==
            get_pow_of_dim(bd, eagine::units::electric_current()) * 2 -
              get_pow_of_dim(bd, eagine::units::length()) * 2 -
              get_pow_of_dim(bd, eagine::units::mass()) +
              get_pow_of_dim(bd, eagine::units::time()) * 3,
          "2");
    });
}
//------------------------------------------------------------------------------
// electric_resistance
//------------------------------------------------------------------------------
void dimension_electric_resistance(auto& s) {
    eagitest::case_ test{s, 16, "electric resistance"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_resistance()) ==
            get_pow_of_dim(bd, eagine::units::electric_tension()) -
              get_pow_of_dim(bd, eagine::units::electric_current()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::electrical_resistance()) ==
            get_pow_of_dim(bd, eagine::units::dimensionless()) -
              get_pow_of_dim(bd, eagine::units::electrical_conductance()),
          "2");
    });
}
//------------------------------------------------------------------------------
// magnetic_flux
//------------------------------------------------------------------------------
void dimension_magnetic_flux(auto& s) {
    eagitest::case_ test{s, 17, "magnetic_flux"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_flux()) ==
            get_pow_of_dim(bd, eagine::units::energy()) -
              get_pow_of_dim(bd, eagine::units::electric_current()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_flux()) ==
            get_pow_of_dim(bd, eagine::units::electric_tension()) +
              get_pow_of_dim(bd, eagine::units::time()),
          "2");
        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_flux()) ==
            get_pow_of_dim(bd, eagine::units::length()) * 2 -
              get_pow_of_dim(bd, eagine::units::time()) * 2 +
              get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::electric_current()),
          "3");
    });
}
//------------------------------------------------------------------------------
// magnetic_field_strength
//------------------------------------------------------------------------------
void dimension_magnetic_field_strength(auto& s) {
    eagitest::case_ test{s, 18, "magnetic field strength"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_field_strength()) ==
            get_pow_of_dim(bd, eagine::units::magnetic_flux()) -
              get_pow_of_dim(bd, eagine::units::area()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_field_strength()) ==
            get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::electric_current()) -
              get_pow_of_dim(bd, eagine::units::time()) * 2,
          "2");
    });
}
//------------------------------------------------------------------------------
// inductance
//------------------------------------------------------------------------------
void dimension_inductance(auto& s) {
    eagitest::case_ test{s, 19, "inductance"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_field_strength()) ==
            get_pow_of_dim(bd, eagine::units::magnetic_flux()) -
              get_pow_of_dim(bd, eagine::units::area()),
          "1");

        test.check(
          get_pow_of_dim(bd, eagine::units::magnetic_field_strength()) ==
            get_pow_of_dim(bd, eagine::units::mass()) -
              get_pow_of_dim(bd, eagine::units::electric_current()) -
              get_pow_of_dim(bd, eagine::units::time()) * 2,
          "2");
    });
}
//------------------------------------------------------------------------------
// frequency
//------------------------------------------------------------------------------
void dimension_frequency(auto& s) {
    eagitest::case_ test{s, 20, "frequency"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::frequency()) ==
            get_pow_of_dim(bd, eagine::units::number_of_cycles()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");
    });
}
//------------------------------------------------------------------------------
// radioactivity
//------------------------------------------------------------------------------
void dimension_radioactivity(auto& s) {
    eagitest::case_ test{s, 21, "radioactivity"};
    eagine::units::base::for_each_dim([&](auto bd) {
        test.check(
          get_pow_of_dim(bd, eagine::units::radioactivity()) ==
            get_pow_of_dim(bd, eagine::units::number_of_decays()) -
              get_pow_of_dim(bd, eagine::units::time()),
          "1");
    });
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "dimension", 21};
    test.once(dimension_area);
    test.once(dimension_volume);
    test.once(dimension_mass_density);
    test.once(dimension_velocity);
    test.once(dimension_angular_velocity);
    test.once(dimension_acceleration);
    test.once(dimension_momentum);
    test.once(dimension_force);
    test.once(dimension_energy);
    test.once(dimension_power);
    test.once(dimension_pressure);
    test.once(dimension_electric_charge);
    test.once(dimension_electric_tension);
    test.once(dimension_electric_capacitance);
    test.once(dimension_electric_conductance);
    test.once(dimension_electric_resistance);
    test.once(dimension_magnetic_flux);
    test.once(dimension_magnetic_field_strength);
    test.once(dimension_inductance);
    test.once(dimension_frequency);
    test.once(dimension_radioactivity);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
