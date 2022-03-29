/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_OFFSET_SPAN_HPP
#define EAGINE_MEMORY_OFFSET_SPAN_HPP

#include "offset_ptr.hpp"
#include "span.hpp"

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Basic alias for basic_span with basic_offset_pointer.
/// @ingroup memory
template <typename T, typename O = span_size_t, typename S = span_size_t>
using basic_offset_span = memory::basic_span<T, basic_offset_ptr<T, O>, S>;
//------------------------------------------------------------------------------
/// @brief Default alias for basic_offset_span.
/// @ingroup memory
template <typename T>
using offset_span = basic_offset_span<T>;
//------------------------------------------------------------------------------
template <typename T, typename O>
auto view_one(basic_offset_ptr<T, O> ptr) -> memory::
  basic_span<std::add_const_t<T>, basic_offset_ptr<std::add_const_t<T>, O>, O> {
    return {ptr, O(1)};
}
//------------------------------------------------------------------------------
template <typename T, typename O>
auto cover_one(basic_offset_ptr<T, O> ptr)
  -> memory::basic_span<T, basic_offset_ptr<T, O>, O> requires(
    !std::is_const_v<T>) {
    return {ptr, O(1)};
}
//------------------------------------------------------------------------------
/// @brief Converts argument to span using an offset pointer type.
/// @ingroup memory
/// @see absolute
template <typename T, typename P, typename S>
static constexpr auto relative(basic_span<T, P, S> spn) noexcept
  -> basic_span<T, basic_offset_ptr<T, std::make_signed_t<S>>, S> {
    return {spn};
}
//------------------------------------------------------------------------------
} // namespace eagine::memory

#endif // EAGINE_MEMORY_OFFSET_SPAN_HPP
