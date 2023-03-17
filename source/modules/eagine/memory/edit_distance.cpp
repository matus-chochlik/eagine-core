/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:edit_distance;

import eagine.core.types;
import :span;
import std;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Calculates the edit distance of @p s1 and @p s2 according to get_distance.
/// @ingroup type_utils
export template <
  typename Result,
  typename T,
  typename P1,
  typename S1,
  typename P2,
  typename S2,
  typename BinaryFunction>
auto basic_edit_distance(
  const basic_span<const T, P1, S1> s1,
  const basic_span<const T, P2, S2> s2,
  BinaryFunction get_distance) noexcept -> Result {
    const auto l1 = s1.size();
    const auto l2 = s2.size();

    std::vector<Result> column;
    column.resize(integer(l1 + 1));
    std::iota(column.begin(), column.end(), Result(0));

    for(span_size_t x = 1; x <= l2; x++) {
        column[0] = Result(x);
        auto prev_diagonal = Result(x) - 1;

        for(span_size_t y = 1; y <= l1; y++) {
            const auto curr_diagonal = column[y];
            const auto prev_min = std::min(column[y] + 1, column[y - 1] + 1);
            const auto new_dist =
              prev_diagonal + Result(get_distance(s1[y - 1], s2[x - 1]));
            column[y] = std::min(prev_min, new_dist);
            prev_diagonal = curr_diagonal;
        }
    }
    return column[l1];
}
//------------------------------------------------------------------------------
export template <typename T, typename P1, typename S1, typename P2, typename S2>
auto default_edit_distance(
  const basic_span<const T, P1, S1> s1,
  const basic_span<const T, P2, S2> s2) noexcept -> span_size_t {
    return basic_edit_distance<span_size_t>(
      s1, s2, [](auto e1, auto e2) { return e1 == e2 ? 0 : 1; });
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
