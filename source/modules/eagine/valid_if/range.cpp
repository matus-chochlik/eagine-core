/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:range;

import eagine.core.types;
import :decl;
import :compare;

namespace eagine {
//------------------------------------------------------------------------------
template <typename R>
struct range_index_type {
    using type = typename R::size_type;
};
//------------------------------------------------------------------------------
template <typename R>
using range_index_t = typename range_index_type<R>::type;
//------------------------------------------------------------------------------
export template <typename R>
using any_range_position = valid_if_nonnegative<range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R>
using valid_range_index = valid_if_lt_size_ge0<R, range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R>
using valid_range_position = valid_if_le_size_ge0<R, range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R, typename T>
constexpr auto range_index(const T i) noexcept {
    return limit_cast<range_index_t<R>>(i);
}
//------------------------------------------------------------------------------
} // namespace eagine

