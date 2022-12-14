/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:bitfield;

import eagine.core.types;
import :enumerators;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Bit, typename Selector>
[[nodiscard]] auto count_all_bits(
  const bitfield<Bit>&,
  const Selector sel) noexcept -> span_size_t
    requires(mapped_enum<Bit, Selector>)
{
    return enumerator_count(std::type_identity<Bit>{}, sel);
}
//------------------------------------------------------------------------------
export template <typename Bit>
[[nodiscard]] auto count_all_bits(const bitfield<Bit>&) noexcept -> span_size_t
    requires(mapped_enum<Bit, default_selector_t>)
{
    return enumerator_count(std::type_identity<Bit>{}, default_selector);
}
//------------------------------------------------------------------------------
export template <typename Bit, typename Selector>
[[nodiscard]] auto count_set_bits(
  const bitfield<Bit> bits,
  const Selector sel) noexcept -> span_size_t
    requires(mapped_enum<Bit, Selector>)
{
    span_size_t result{0};
    for(const auto& info : enumerator_mapping(std::type_identity<Bit>{}, sel)) {
        result += (bits.has(info.enumerator) ? 1 : 0);
    }
    return result;
}
//------------------------------------------------------------------------------
export template <default_mapped_enum Bit>
[[nodiscard]] auto count_set_bits(const bitfield<Bit> bits) noexcept
  -> span_size_t {
    return count_set_bits(bits, default_selector);
}
//------------------------------------------------------------------------------
export template <typename Function, typename Bit, typename Selector>
void for_each_bit(
  Function function,
  const bitfield<Bit> bits,
  const Selector sel) noexcept
    requires(mapped_enum<Bit, Selector>)
{
    for(const auto& info : enumerator_mapping(std::type_identity<Bit>{}, sel)) {
        function(bits, info);
    }
}
//------------------------------------------------------------------------------
export template <typename Function, default_mapped_enum Bit>
void for_each_bit(Function function, const bitfield<Bit> bits) noexcept {
    for_each_bit(function, bits, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
