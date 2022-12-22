/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
import <algorithm>;
//------------------------------------------------------------------------------
// 1
//------------------------------------------------------------------------------
void units_base_dim_numbers(auto& s) {
    eagitest::case_ test{s, 1, "1"};
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
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "base", 1};
    test.once(units_base_dim_numbers);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

