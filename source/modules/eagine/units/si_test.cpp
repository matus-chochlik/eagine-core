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
void units_si_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    using q_b = tagged_quantity<float, bit>;
    using q_B = tagged_quantity<float, byte>;
    using q_kB = tagged_quantity<float, kilobyte>;
    using q_KiB = tagged_quantity<float, kibibyte>;
    using q_MB = tagged_quantity<float, megabyte>;
    using q_MiB = tagged_quantity<float, mebibyte>;

    test.check_close(value(q_b(1)), value(q_b(q_B(1) / 8)), "b");
    test.check_close(value(q_B(1)), value(q_B(q_b(1) * 8)), "B");
    test.check_close(value(q_kB(1)), value(q_kB(q_B(1000))), "kB");
    test.check_close(value(q_kB(1)), value(q_kB(q_b(8000))), "kb");
    test.check_close(value(q_KiB(1)), value(q_KiB(q_B(1024))), "KiB");
    test.check_close(value(q_KiB(1)), value(q_KiB(q_b(8192))), "Kib");
    test.check_close(value(q_MB(1)), value(q_MB(q_B(1000000))), "MB");
    test.check_close(value(q_MB(1)), value(q_MB(q_b(8000000))), "Mb");
    test.check_close(value(q_MB(1)), value(q_MB(q_kB(1000))), "MB");
    test.check_close(value(q_MiB(1)), value(q_MiB(q_B(1024 * 1024))), "MiB");
    test.check_close(value(q_MiB(1)), value(q_MiB(q_b(8192 * 1024))), "Mib");
    test.check_close(value(q_MiB(1)), value(q_MiB(q_KiB(1024))), "MiB");
}
//------------------------------------------------------------------------------
void units_si_angle(auto& s) {
    eagitest::case_ test{s, 2, "angle"};

    using namespace eagine;
    using namespace eagine::units;

    using q_pi_rad = tagged_quantity<float, pi_rad>;
    using q_rad = tagged_quantity<float, radian>;
    using q_deg = tagged_quantity<float, degree>;
    using q_grad = tagged_quantity<float, gradian>;
    using q_Q = tagged_quantity<float, quarter>;
    using q_T = tagged_quantity<float, turn>;

    static const auto pi = 3.14159265358979323846;

    test.check_close(value(q_deg(1)), value(q_deg(q_T(1) / 360)), "A");
    test.check_close(value(q_deg(1)), value(q_deg(q_Q(1) / 90)), "B");
    test.check_close(value(q_grad(1)), value(q_grad(q_T(1) / 400)), "C");
    test.check_close(value(q_grad(1)), value(q_grad(q_Q(1) / 100)), "D");
    test.check_close(value(q_Q(1)), value(q_Q(q_T(1) / 4)), "E");
    test.check_close(value(q_T(1)), value(q_T(q_Q(1) * 4)), "F");
    test.check_close(value(q_T(1)), value(q_T(q_pi_rad(1) * 2)), "G");
    test.check_close(
      value(q_grad(1)), value(q_grad(q_deg(1) * 360 / 400)), "H");
    test.check_close(value(q_deg(1)), value(q_deg(q_grad(1) * 400 / 360)), "I");
    test.check_close(value(q_rad(1)), value(q_rad(q_pi_rad(1) / pi)), "J");
    test.check_close(value(q_rad(1)), value(q_rad(q_deg(1) * 180 / pi)), "K");
    test.check_close(value(q_deg(1)), value(q_deg(q_rad(1) * pi / 180)), "J");
}
//------------------------------------------------------------------------------
void units_si_mass(auto& s) {
    eagitest::case_ test{s, 3, "mass"};

    using namespace eagine;
    using namespace eagine::units;

    using q_g = tagged_quantity<float, gram>;
    using q_mg = tagged_quantity<float, milligram>;
    using q_kg = tagged_quantity<float, kilogram>;
    using q_t = tagged_quantity<float, tonne>;

    test.check_close(value(q_mg(1)), value(q_mg(q_g(1) / 1000)), "A");
    test.check_close(value(q_kg(1)), value(q_kg(q_g(1) * 1000)), "B");
    test.check_close(value(q_kg(1)), value(q_kg(q_t(1) / 1000)), "C");

    test.check_close(value(q_t(1)), value(q_t(q_mg(1000) * 1000 * 1000)), "D");
    test.check_close(value(q_mg(1)), value(q_mg(q_kg(0.001f) / 1000)), "E");
}
//------------------------------------------------------------------------------
void units_si_length(auto& s) {
    eagitest::case_ test{s, 4, "length"};

    using namespace eagine;
    using namespace eagine::units;

    using q_m = tagged_quantity<float, meter>;
    using q_mm = tagged_quantity<float, millimeter>;
    using q_cm = tagged_quantity<float, centimeter>;
    using q_km = tagged_quantity<float, kilometer>;

    test.check_close(value(q_mm(1)), value(q_mm(q_m(1) / 1000)), "A");
    test.check_close(value(q_m(1)), value(q_m(q_km(1) / 1000)), "B");
    test.check_close(value(q_km(1)), value(q_km(q_m(1) * 1000)), "C");
    test.check_close(value(q_m(1)), value(q_m(q_mm(1) * 1000)), "D");

    test.check_close(value(q_cm(1)), value(q_cm(q_m(1) / 100)), "E");
    test.check_close(value(q_mm(1)), value(q_mm(q_cm(1) / 10)), "F");
    test.check_close(value(q_cm(1)), value(q_cm(q_mm(1) * 10)), "G");
    test.check_close(value(q_m(1)), value(q_m(q_cm(1) * 100)), "H");

    test.check_close(value(q_mm(1)), value(q_mm(q_km(0.001f) / 1000)), "I");
    test.check_close(value(q_km(1)), value(q_km(q_mm(1000) * 1000)), "J");
}
//------------------------------------------------------------------------------
void units_si_temperature(auto& s) {
    eagitest::case_ test{s, 5, "temperature"};

    using namespace eagine;
    using namespace eagine::units;

    using q_K = tagged_quantity<float, kelvin>;
    using q_mK = tagged_quantity<float, millikelvin>;
    using q_kK = tagged_quantity<float, kilokelvin>;

    test.check_close(value(q_mK(1)), value(q_mK(q_K(1) / 1000)), "A");
    test.check_close(value(q_kK(1)), value(q_kK(q_K(1) * 1000)), "B");
    test.check_close(value(q_mK(1)), value(q_mK(q_kK(0.001f) / 1000)), "C");
}
//------------------------------------------------------------------------------
void units_si_luminous_intensity(auto& s) {
    eagitest::case_ test{s, 6, "luminous intensity"};

    using namespace eagine;
    using namespace eagine::units;

    using q_cd = tagged_quantity<float, candela>;
    using q_kcd = tagged_quantity<float, kilocandela>;

    test.check_close(value(q_kcd(1)), value(q_kcd(q_cd(1) * 1000)), "A");
    test.check_close(value(q_cd(1)), value(q_cd(q_kcd(1) / 1000)), "B");
}
//------------------------------------------------------------------------------
void units_si_amount_of_substance(auto& s) {
    eagitest::case_ test{s, 7, "amount of substance"};

    using namespace eagine;
    using namespace eagine::units;

    using q_mol = tagged_quantity<float, mole>;
    using q_kmol = tagged_quantity<float, kilomole>;

    test.check_close(value(q_kmol(1)), value(q_kmol(q_mol(1) * 1000)), "A");
    test.check_close(value(q_mol(1)), value(q_mol(q_kmol(1) / 1000)), "B");
}
//------------------------------------------------------------------------------
void units_si_time(auto& s) {
    eagitest::case_ test{s, 8, "time"};

    using namespace eagine;
    using namespace eagine::units;

    using q_ms = tagged_quantity<float, millisecond>;
    using q_s = tagged_quantity<float, second>;
    using q_min = tagged_quantity<float, minute>;
    using q_hr = tagged_quantity<float, hour>;
    using q_D = tagged_quantity<float, day>;

    test.check_close(value(q_ms(1)), value(q_ms(q_s(1) / 1000)), "A");
    test.check_close(value(q_s(1)), value(q_s(q_min(1) / 60)), "B");
    test.check_close(value(q_min(1)), value(q_min(q_hr(1) / 60)), "C");
    test.check_close(value(q_s(1)), value(q_s(q_hr(1) / 3600)), "D");
    test.check_close(value(q_hr(1)), value(q_hr(q_D(1) / 24)), "E");
    test.check_close(value(q_min(1)), value(q_min(q_D(1) / 1440)), "F");
}
//------------------------------------------------------------------------------
template <typename U1, typename U2>
void units_si_2_U1_U2(eagitest::case_& test, double r) {
    using namespace eagine;
    auto& rg{test.random()};

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        double x = rg.get_between<double>(-10, 10);

        using q_U1 = tagged_quantity<double, U1>;
        using q_U2 = tagged_quantity<double, U2>;

        q_U1 q1(x);
        q_U2 q2(x / r);

        test.check((q1 > q2) != (q1 <= q2), "A");
        test.check((q1 >= q2) != (q1 < q2), "B");

        test.check_close(value(q1), value(q_U1(q2)), "C");
        test.check_close(value(q_U2(q1)), value(q2), "D");
    }
}
//------------------------------------------------------------------------------
void units_si_2(auto& s) {
    eagitest::case_ test{s, 9, "2"};

    using namespace eagine::units;

    static const auto pi = 3.14159265358979323846;

    // angle
    units_si_2_U1_U2<pi_rad, turn>(test, 2);
    units_si_2_U1_U2<quarter, turn>(test, 4);
    units_si_2_U1_U2<degree, turn>(test, 360);
    units_si_2_U1_U2<degree, quarter>(test, 90);
    units_si_2_U1_U2<degree, pi_rad>(test, 180);
    units_si_2_U1_U2<gradian, turn>(test, 400);
    units_si_2_U1_U2<gradian, quarter>(test, 100);
    units_si_2_U1_U2<gradian, pi_rad>(test, 200);
    units_si_2_U1_U2<radian, pi_rad>(test, pi);
    units_si_2_U1_U2<pi_rad, radian>(test, 1 / pi);

    // mass
    units_si_2_U1_U2<milligram, gram>(test, 1000);
    units_si_2_U1_U2<gram, kilogram>(test, 1000);
    units_si_2_U1_U2<kilogram, tonne>(test, 1000);
    units_si_2_U1_U2<gram, milligram>(test, 0.001);
    units_si_2_U1_U2<kilogram, gram>(test, 0.001);
    units_si_2_U1_U2<tonne, kilogram>(test, 0.001);
    units_si_2_U1_U2<milligram, kilogram>(test, 1000000);
    units_si_2_U1_U2<milligram, tonne>(test, 1000000000);

    // length
    units_si_2_U1_U2<millimeter, meter>(test, 1000);
    units_si_2_U1_U2<meter, kilometer>(test, 1000);
    units_si_2_U1_U2<centimeter, meter>(test, 100);
    units_si_2_U1_U2<centimeter, decimeter>(test, 10);
    units_si_2_U1_U2<millimeter, centimeter>(test, 10);
    units_si_2_U1_U2<decimeter, meter>(test, 10);
    units_si_2_U1_U2<meter, millimeter>(test, 0.001);
    units_si_2_U1_U2<kilometer, meter>(test, 0.001);
    units_si_2_U1_U2<meter, centimeter>(test, 0.01);
    units_si_2_U1_U2<centimeter, millimeter>(test, 0.1);
    units_si_2_U1_U2<decimeter, centimeter>(test, 0.1);

    // time
    units_si_2_U1_U2<millisecond, second>(test, 1000);
    units_si_2_U1_U2<second, minute>(test, 60);
    units_si_2_U1_U2<second, hour>(test, 60 * 60);
    units_si_2_U1_U2<minute, hour>(test, 60);
    units_si_2_U1_U2<second, day>(test, 24 * 60 * 60);
    units_si_2_U1_U2<minute, day>(test, 24 * 60);
    units_si_2_U1_U2<hour, day>(test, 24);
    units_si_2_U1_U2<second, millisecond>(test, 0.001);
    units_si_2_U1_U2<minute, second>(test, 1 / 60.0);
    units_si_2_U1_U2<hour, minute>(test, 1 / 60.0);
    units_si_2_U1_U2<day, hour>(test, 1 / 24.0);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "si", 8};
    test.once(units_si_1);
    test.once(units_si_2);
    test.once(units_si_angle);
    test.once(units_si_mass);
    test.once(units_si_length);
    test.once(units_si_temperature);
    test.once(units_si_luminous_intensity);
    test.once(units_si_amount_of_substance);
    test.once(units_si_time);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

