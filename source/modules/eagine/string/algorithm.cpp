/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:algorithm;

import std;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Replaces all occurrences of @p from with @p to in the given string.
/// @ingroup string_utils
export template <
  typename C,
  typename T,
  typename A,
  typename P1,
  typename P2,
  typename S1,
  typename S2>
auto string_replace(
  std::basic_string<C, T, A>& str,
  const memory::basic_span<const C, P1, S1> from,
  const memory::basic_span<const C, P2, S2> to) noexcept
  -> std::basic_string<C, T, A>& {
    std::string::size_type index = 0;
    while(true) {
        index = str.find(from.data(), index, from.size());
        if(index == std::string::npos) {
            break;
        }
        str.replace(index, from.size(), to.data(), to.size());
        index += to.size();
    }
    return str;
}
//------------------------------------------------------------------------------
/// @brief Replaces all occurrences of the string specifed in the dictionary.
/// @ingroup string_utils
export template <
  typename C,
  typename T,
  typename A,
  typename P1,
  typename P2,
  typename S1,
  typename S2>
auto string_replace(
  std::basic_string<C, T, A>& str,
  const span<const std::tuple<
    memory::basic_string_span<const C, P1, S1>,
    memory::basic_string_span<const C, P2, S2>>> dict) noexcept
  -> std::basic_string<C, T, A>& {
    for(auto& [from, to] : dict) {
        string_replace(str, from, to);
    }
    return str;
}
//------------------------------------------------------------------------------
} // namespace eagine
