/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units.framework;

import eagine.core.types;
import :base;
export import :quantity;
import <type_traits>;
import <utility>;

namespace eagine {
namespace units {
//------------------------------------------------------------------------------
export template <typename System>
struct system_id;

export struct si;
//------------------------------------------------------------------------------
} // namespace units
export using units::tagged_quantity;

export template <typename T1, typename U1, typename T2, typename U2>
struct equal_cmp<units::tagged_quantity<T1, U1>, units::tagged_quantity<T2, U2>> {

    static_assert(units::is_convertible_v<U2, U1>);

    static constexpr auto check(
      const tagged_quantity<T1, U1>& l,
      const tagged_quantity<T2, U2>& r) noexcept -> bool {
        return are_equal(value(l), units::value_conv<U2, U1>()(value(r)));
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
