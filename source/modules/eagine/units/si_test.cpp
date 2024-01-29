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
// 1
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
// angle
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
// mass
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
// length
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
// temperature
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
// luminous intensity
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
// amount of substance
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
// time
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
// 2
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
// 3
//------------------------------------------------------------------------------
template <typename T>
void units_si_3_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto l1{rg.get_between<T>(1, 1000)};
    const auto l2{rg.get_between<T>(1, 1000)};
    const auto l3{rg.get_between<T>(1, 1000)};

    tagged_quantity<T, meter> ql1_m(l1);
    tagged_quantity<T, meter> ql2_m(l2);
    tagged_quantity<T, meter> ql3_m(l3);

    tagged_quantity<T, unit<area, si>> qa12_m2 = ql1_m * ql2_m;
    tagged_quantity<T, unit<area, si>> qa13_m2 = ql1_m * ql3_m;
    tagged_quantity<T, unit<area, si>> qa23_m2 = ql2_m * ql3_m;

    tagged_quantity<T, unit<volume, si>> qv123_m3 = ql1_m * ql2_m * ql3_m;

    test.check_close(value(ql1_m) * value(ql2_m), value(qa12_m2), "A");
    test.check_close(value(ql1_m) * value(ql3_m), value(qa13_m2), "B");
    test.check_close(value(ql2_m) * value(ql3_m), value(qa23_m2), "C");

    test.check_close(
      value(ql1_m) * value(ql2_m) * value(ql3_m), value(qv123_m3), "D");

    test.check_close(value(qa12_m2) * value(ql3_m), value(qv123_m3), "E");
    test.check_close(value(qa13_m2) * value(ql2_m), value(qv123_m3), "F");
    test.check_close(value(ql1_m) * value(qa23_m2), value(qv123_m3), "G");
    test.check_close(value(qa12_m2) / value(ql2_m), value(ql1_m), "H");
    test.check_close(value(qa23_m2) / value(ql3_m), value(ql2_m), "I");
    test.check_close(value(qv123_m3) / value(ql3_m), value(qa12_m2), "J");
    test.check_close(value(qv123_m3) / value(qa13_m2), value(ql2_m), "K");
}
//------------------------------------------------------------------------------
void units_si_3(unsigned, auto& s) {
    eagitest::case_ test{s, 10, "3"};
    units_si_3_T<float>(test);
    units_si_3_T<double>(test);
}
//------------------------------------------------------------------------------
// 4
//------------------------------------------------------------------------------
template <typename T>
void units_si_4_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto l1{rg.get_between<T>(1, 1000)};
    const auto l2{rg.get_between<T>(1, 1000)};
    const auto l3{rg.get_between<T>(1, 1000)};

    tagged_quantity<T, meter> ql1_m(l1);
    tagged_quantity<T, centimeter> ql2_cm(l2);
    tagged_quantity<T, millimeter> ql3_mm(l3);

    tagged_quantity<T, unit<area, si>> qa22_m2 = ql2_cm * ql2_cm;
    tagged_quantity<T, unit<area, si>> qa12_m2 = ql1_m * ql2_cm;
    tagged_quantity<T, unit<area, si>> qa13_m2 = ql1_m * ql3_mm;
    tagged_quantity<T, unit<area, si>> qa23_m2 = ql2_cm * ql3_mm;

    tagged_quantity<T, unit<volume, si>> qv123_m3 = ql1_m * ql2_cm * ql3_mm;

    test.check_close(
      value(ql2_cm) * value(ql2_cm) / 10000, value(qa22_m2), "A");
    test.check_close(
      value(ql2_cm) * value(ql2_cm), value(ql2_cm * ql2_cm), "B");
    test.check_close(value(ql1_m) * value(ql2_cm) / 100, value(qa12_m2), "C");
    test.check_close(value(ql1_m) * value(ql1_m), value(ql1_m * ql1_m), "D");
    test.check_close(value(ql1_m) * value(ql3_mm) / 1000, value(qa13_m2), "E");

    test.check_close(
      value(ql3_mm) * value(ql3_mm), value(ql3_mm * ql3_mm), "F");

    test.check_close(
      value(ql1_m) * value(ql2_cm) * value(ql3_mm) / 100000,
      value(qv123_m3),
      "G");

    test.check_close(value(qv123_m3) / value(ql1_m), value(qa23_m2), "H");
    test.check_close(
      value(qv123_m3) / (value(ql2_cm) / 100), value(qa13_m2), "I");
    test.check_close(
      value(qv123_m3) / (value(ql3_mm) / 1000), value(qa12_m2), "J");
    test.check_close(
      value(qv123_m3) / value(qa12_m2), value(ql3_mm) / 1000, "K");
    test.check_close(
      value(qv123_m3) / value(qa13_m2), value(ql2_cm) / 100, "L");
    test.check_close(value(qv123_m3) / value(qa23_m2), value(ql1_m), "M");
}
//------------------------------------------------------------------------------
void units_si_4(unsigned, auto& s) {
    eagitest::case_ test{s, 11, "4"};
    units_si_4_T<float>(test);
    units_si_4_T<double>(test);
}
//------------------------------------------------------------------------------
// 5
//------------------------------------------------------------------------------
template <typename T>
void units_si_5_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto t1{rg.get_between<T>(1, 1000)};
    const auto t2{rg.get_between<T>(1, 1000)};
    const auto l1{rg.get_between<T>(1, 1000)};
    const auto l2{rg.get_between<T>(1, 1000)};
    const auto m1{rg.get_between<T>(1, 1000)};

    tagged_quantity<T, meter> ql1_m(l1);
    tagged_quantity<T, kilometer> ql2_km(l2);
    tagged_quantity<T, second> qt1_s(t1);
    tagged_quantity<T, hour> qt2_h(t2);
    tagged_quantity<T, kilogram> qm1_kg(m1);
    tagged_quantity<T, unit<velocity, si>> qv1_m_s = ql1_m / qt1_s;
    tagged_quantity<T, unit<velocity, si>> qv2_m_s = ql2_km / qt1_s;
    tagged_quantity<T, unit<velocity, si>> qv3_m_s = ql2_km / qt2_h;
    tagged_quantity<T, unit<acceleration, si>> qa1_m_s2 = qv1_m_s / qt1_s;
    tagged_quantity<T, newton> qF1_N = qm1_kg * qa1_m_s2;
    tagged_quantity<T, pascal> qp1_Pa = qF1_N / (ql1_m * ql1_m);
    tagged_quantity<T, kilonewton> qF2_kN(qF1_N);
    tagged_quantity<T, joule> qE1_J(qF1_N * ql2_km);
    tagged_quantity<T, kilojoule> qE2_kJ(qm1_kg * qa1_m_s2 * ql2_km);
    tagged_quantity<T, watt> qP1_W(qE1_J / qt2_h);
    tagged_quantity<T, kilowatt> qP2_kW(qE2_kJ / qt2_h);
    tagged_quantity<T, megawatt> qP3_MW(qm1_kg * qa1_m_s2 * ql2_km / qt2_h);

    test.check_close(value(ql1_m) / value(qt1_s), value(qv1_m_s), "A");

    test.check_close(value(ql2_km) / value(qt1_s), value(qv2_m_s) / 1000, "B");

    test.check_close(value(ql2_km) / value(qt2_h), value(qv3_m_s) / 3600, "C");

    test.check_close(value(ql2_km) / value(qt2_h), value(ql2_km / qt2_h), "D");

    test.check_close(
      value(ql1_m) / value(qt1_s) / value(qt1_s), value(qa1_m_s2), "E");

    test.check_close(
      value(qm1_kg) * value(qv1_m_s) / value(qt1_s), value(qF1_N), "F");

    test.check_close(
      value(qm1_kg) / (value(qt1_s) * value(qt1_s) * value(ql1_m)),
      value(qp1_Pa),
      "G");

    test.check_close(
      (value(ql2_km) * 1000) * value(qm1_kg) * value(qv1_m_s) / value(qt1_s),
      value(qE1_J),
      "H");

    test.check_close(value(qE2_kJ) * 1000, value(qE1_J), "I");

    test.check_close(
      value(qF1_N) * (value(ql2_km) * 1000) / (value(qt2_h) * 3600),
      value(qP1_W),
      "J");

    test.check_close(value(qP2_kW), value(qP1_W), "K");
    test.check_close(value(qP3_MW) * 1000000, value(qP1_W), "L");
}
//------------------------------------------------------------------------------
void units_si_5(unsigned, auto& s) {
    eagitest::case_ test{s, 12, "5"};
    units_si_5_T<float>(test);
    units_si_5_T<double>(test);
}
//------------------------------------------------------------------------------
// 6
//------------------------------------------------------------------------------
template <typename T>
void units_si_6_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto t1{rg.get_any<T>()};
    const auto cc1{rg.get_any<T>()};

    tagged_quantity<T, second> qt1_s(t1);
    tagged_quantity<T, hertz> qf1_Hz(cc1 / t1);
    tagged_quantity<T, kilohertz> qf2_kHz(qf1_Hz);

    test.check_close(value(qf1_Hz * qt1_s), cc1, "A");
    test.check_close(value(qf1_Hz) / 1000, value(qf2_kHz), "B");
    test.check_close(value(qf2_kHz) * value(qt1_s), cc1 / 1000, "C");
}
//------------------------------------------------------------------------------
void units_si_6(unsigned, auto& s) {
    eagitest::case_ test{s, 13, "6"};
    units_si_6_T<float>(test);
    units_si_6_T<double>(test);
}
//------------------------------------------------------------------------------
// 7
//------------------------------------------------------------------------------
template <typename T>
void units_si_7_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto t1{rg.get_any<T>()};
    const auto dc1{rg.get_any<T>()};

    tagged_quantity<T, second> qt1_s(t1);
    tagged_quantity<T, becquerel> qr1_Bq(dc1 / t1);
    tagged_quantity<T, megabecquerel> qr2_MBq(qr1_Bq);
    tagged_quantity<T, unit<number_of_decays, si>> qnod1(qr2_MBq * qt1_s);

    test.check_close(value(qr1_Bq * qt1_s), dc1, "A");
    test.check_close(value(qr1_Bq) / 1000000, value(qr2_MBq), "B");
    test.check_close(value(qr2_MBq) * value(qt1_s), dc1 / 1000000, "C");
    test.check_close(value(qnod1), dc1, "D");
}
//------------------------------------------------------------------------------
void units_si_7(unsigned, auto& s) {
    eagitest::case_ test{s, 14, "7"};
    units_si_7_T<float>(test);
    units_si_7_T<double>(test);
}
//------------------------------------------------------------------------------
// 8
//------------------------------------------------------------------------------
template <typename T>
void units_si_8_T(eagitest::case_& test) {
    using namespace eagine;
    auto& rg{test.random()};

    using eagine::tagged_quantity;
    using namespace eagine::units;

    const auto l1{rg.get_between<T>(1, 1000)};
    const auto t1{rg.get_between<T>(1, 1000)};
    const auto I1{rg.get_between<T>(1, 1000)};
    const auto E1{rg.get_between<T>(1, 1000)};

    tagged_quantity<T, meter> ql1_m(l1);
    tagged_quantity<T, second> qt1_s(t1);
    tagged_quantity<T, hour> qt2_h(t1 / 3600);
    tagged_quantity<T, ampere> qI1_A(I1);
    tagged_quantity<T, coulomb> qQ1_C = qI1_A * qt1_s;
    tagged_quantity<T, kiloampere> qI1_kA = qQ1_C / qt2_h;
    tagged_quantity<T, milliampere> qI1_mA = qI1_A;
    tagged_quantity<T, joule> qE1_J(E1);
    tagged_quantity<T, volt> qU1_V(qE1_J / qQ1_C);
    tagged_quantity<T, watt> qP1_W(qU1_V * qI1_kA);
    tagged_quantity<T, millifarad> qC1_mF(qQ1_C / qU1_V);
    tagged_quantity<T, millisiemens> qG1_mS(qI1_mA / qU1_V);
    tagged_quantity<T, kiloohm> qR1_kO(qU1_V / qI1_mA);
    tagged_quantity<T, weber> qPhi1_Wb(qE1_J / qI1_kA);
    tagged_quantity<T, tesla> qB1_T(qPhi1_Wb / (ql1_m * ql1_m));
    tagged_quantity<T, millihenry> qL1_mH(qPhi1_Wb / qI1_kA);

    test.check_close(value(qI1_A) / 1000, value(qI1_kA), "A");
    test.check_close(value(qI1_kA) * 1000000, value(qI1_mA), "B");
    test.check_close(value(qE1_J) / value(qQ1_C), value(qU1_V), "C");
    test.check_close(value(qE1_J) / value(qt1_s), value(qP1_W), "D");
    test.check_close(1 / value(qG1_mS), value(qR1_kO), "E");
    test.check_close(value(qt1_s) * value(qG1_mS), value(qC1_mF), "F");
    test.check_close(value(qt1_s) / value(qR1_kO), value(qC1_mF), "G");
    test.check_close(value(qU1_V) * value(qt1_s), value(qPhi1_Wb), "H");
    test.check_close(value(qE1_J) / value(qI1_A), value(qPhi1_Wb), "I");
    test.check_close(
      value(qU1_V) * value(qt1_s) / (value(ql1_m) * value(ql1_m)),
      value(qB1_T),
      "J");
    test.check_close(value(qPhi1_Wb) / value(qI1_kA), value(qL1_mH), "K");
}
//------------------------------------------------------------------------------
void units_si_8(unsigned, auto& s) {
    eagitest::case_ test{s, 15, "8"};
    units_si_8_T<float>(test);
    units_si_8_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "si", 15};
    test.once(units_si_1);
    test.once(units_si_angle);
    test.once(units_si_mass);
    test.once(units_si_length);
    test.once(units_si_temperature);
    test.once(units_si_luminous_intensity);
    test.once(units_si_amount_of_substance);
    test.once(units_si_time);
    test.once(units_si_2);
    test.repeat(50, units_si_3);
    test.repeat(50, units_si_4);
    test.repeat(50, units_si_5);
    test.repeat(50, units_si_6);
    test.repeat(50, units_si_7);
    test.repeat(50, units_si_8);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

