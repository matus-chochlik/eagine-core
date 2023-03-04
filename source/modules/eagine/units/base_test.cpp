/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
import std;
//------------------------------------------------------------------------------
// dimension numbers
//------------------------------------------------------------------------------
void units_base_dim_numbers(auto& s) {
    eagitest::case_ test{s, 1, "dim numbers"};
    using namespace eagine::units;

    int dim_nums[] = {
      get_number(base::angle()),
      get_number(base::solid_angle()),
      get_number(base::length()),
      get_number(base::mass()),
      get_number(base::time()),
      get_number(base::temperature()),
      get_number(base::electric_current()),
      get_number(base::number_of_cycles()),
      get_number(base::number_of_decays()),
      get_number(base::luminous_intensity()),
      get_number(base::amount_of_substance()),
      get_number(base::amount_of_information())};

    using std::begin;
    using std::end;
    using std::unique;

    test.check(
      unique(begin(dim_nums), end(dim_nums)) == end(dim_nums),
      "unique dim numbers");
}
//------------------------------------------------------------------------------
// pow
//------------------------------------------------------------------------------
template <typename Dimension, typename BaseDim, typename TestedBaseDim>
void units_dimension_pow1_d_bd(eagitest::case_& test, TestedBaseDim) {
    test.check_equal(
      get_pow_of_dim(TestedBaseDim(), Dimension()),
      int(std::is_same_v<BaseDim, TestedBaseDim>),
      "pow of dim");
}
//------------------------------------------------------------------------------
void units_base_dim_powers(auto& s) {
    eagitest::case_ test{s, 2, "dim powers"};
    using namespace eagine::units;

    const auto tester = [&test](auto tested_base_dim) {
        units_dimension_pow1_d_bd<angle, base::angle>(test, tested_base_dim);
        units_dimension_pow1_d_bd<solid_angle, base::solid_angle>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<length, base::length>(test, tested_base_dim);
        units_dimension_pow1_d_bd<mass, base::mass>(test, tested_base_dim);
        units_dimension_pow1_d_bd<eagine::units::time, base::time>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<temperature, base::temperature>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<electric_current, base::electric_current>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<number_of_cycles, base::number_of_cycles>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<number_of_decays, base::number_of_decays>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<luminous_intensity, base::luminous_intensity>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<amount_of_substance, base::amount_of_substance>(
          test, tested_base_dim);
        units_dimension_pow1_d_bd<
          amount_of_information,
          base::amount_of_information>(test, tested_base_dim);
    };

    base::for_each_dim(tester);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "base", 2};
    test.once(units_base_dim_numbers);
    test.once(units_base_dim_powers);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

