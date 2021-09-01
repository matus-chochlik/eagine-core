/// @example eagine/reflect_enum.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/reflect/enumerators.hpp>
#include <iostream>

namespace eagine {

enum class example_enum {
    value_a,
    value_b,
    value_c,
    value_d,
    value_e,
    value_f,
    value_g,
    value_h,
    value_i,
    value_j,
    value_k,
    value_l,
    value_m,
    value_n,
    value_o,
    value_p,
    value_q,
    value_r,
    value_s,
    value_t,
    value_u,
    value_v,
    value_w,
    value_x,
    value_y,
    value_z
};

#if !EAGINE_CXX_REFLECTION
template <typename Selector>
constexpr auto enumerator_mapping(
  const type_identity<example_enum>,
  const Selector) noexcept {
    return enumerator_map_type<example_enum, 26>{
      {{"value_a", example_enum::value_a}, {"value_b", example_enum::value_b},
       {"value_c", example_enum::value_c}, {"value_d", example_enum::value_d},
       {"value_e", example_enum::value_e}, {"value_f", example_enum::value_f},
       {"value_g", example_enum::value_g}, {"value_h", example_enum::value_h},
       {"value_i", example_enum::value_i}, {"value_j", example_enum::value_j},
       {"value_k", example_enum::value_k}, {"value_l", example_enum::value_l},
       {"value_m", example_enum::value_m}, {"value_n", example_enum::value_n},
       {"value_o", example_enum::value_o}, {"value_p", example_enum::value_p},
       {"value_q", example_enum::value_q}, {"value_r", example_enum::value_r},
       {"value_s", example_enum::value_s}, {"value_t", example_enum::value_t},
       {"value_u", example_enum::value_u}, {"value_v", example_enum::value_v},
       {"value_w", example_enum::value_w}, {"value_x", example_enum::value_x},
       {"value_y", example_enum::value_y}, {"value_z", example_enum::value_z}}};
}
#endif

} // namespace eagine

auto main() -> int {
    using namespace eagine;

    const type_identity<example_enum> tid{};
    std::cout << "count: " << enumerator_count(tid) << std::endl;

    for_each_enumerator(
      [](const auto& info) {
          std::cout << info.name << ": " << info.value << std::endl;
      },
      type_identity<example_enum>{});

    return 0;
}
