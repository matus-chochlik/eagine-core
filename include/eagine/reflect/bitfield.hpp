/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_BITFIELD_HPP
#define EAGINE_REFLECT_BITFIELD_HPP

#include "../bitfield.hpp"
#include "enumerators.hpp"

namespace eagine {
//------------------------------------------------------------------------------
template <typename Bit, typename Selector>
inline auto count_all_bits(const bitfield<Bit>&, const Selector sel) noexcept
  -> span_size_t requires(has_enumerator_mapping_v<Bit, Selector>) {
    return enumerator_count(type_identity<Bit>{}, sel);
}
//------------------------------------------------------------------------------
template <typename Bit>
inline auto count_all_bits(const bitfield<Bit>&) noexcept -> span_size_t
  requires(has_enumerator_mapping_v<Bit, default_selector_t>) {
    return enumerator_count(type_identity<Bit>{}, default_selector);
}
//------------------------------------------------------------------------------
template <typename Bit, typename Selector>
inline auto count_set_bits(const bitfield<Bit> bits, const Selector sel) noexcept
  -> span_size_t requires(has_enumerator_mapping_v<Bit, Selector>) {
    span_size_t result{0};
    for(const auto& info : enumerator_mapping(type_identity<Bit>{}, sel)) {
        result += (bits.has(info.enumerator) ? 1 : 0);
    }
    return result;
}
//------------------------------------------------------------------------------
template <typename Bit>
inline auto count_set_bits(const bitfield<Bit> bits) noexcept -> span_size_t
  requires(has_enumerator_mapping_v<Bit, default_selector_t>) {
    return count_set_bits(bits, default_selector);
}
//------------------------------------------------------------------------------
template <typename Function, typename Bit, typename Selector>
inline void for_each_bit(
  Function function,
  const bitfield<Bit> bits,
  const Selector sel) noexcept
  requires(has_enumerator_mapping_v<Bit, Selector>) {
    for(const auto& info : enumerator_mapping(type_identity<Bit>{}, sel)) {
        function(bits, info);
    }
}
//------------------------------------------------------------------------------
template <typename Function, typename Bit>
inline void for_each_bit(Function function, const bitfield<Bit> bits) noexcept
  requires(has_enumerator_mapping_v<Bit, default_selector_t>) {
    for_each_bit(function, bits, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif
