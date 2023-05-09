/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units;

import std;
import eagine.core.types;
export import :base;
export import :dimension;
export import :unit;
export import :si;
export import :quantity;
export import :dimensions;
export import :strings;

namespace eagine {
//------------------------------------------------------------------------------
export using units::tagged_quantity;

export template <typename T1, typename U1, typename T2, typename U2>
struct equal_cmp<units::tagged_quantity<T1, U1>, units::tagged_quantity<T2, U2>> {

    static_assert(units::is_convertible_v<U2, U1>);

    constexpr auto check(
      const tagged_quantity<T1, U1>& l,
      const tagged_quantity<T2, U2>& r) noexcept -> bool {
        return are_equal(value(l), units::value_conv<U2, U1>()(value(r)));
    }
};
//------------------------------------------------------------------------------
/// @brief Alias for type storing duration values in seconds.
/// @ingroup units
/// @see seconds_
export template <typename T>
using seconds_t = tagged_quantity<T, units::second>;

/// @brief Creates a tagged quantity storing value in seconds.
/// @ingroup units
export template <typename T>
constexpr auto seconds_(const T value) noexcept -> seconds_t<T> {
    return seconds_t<T>{value};
}

/// @brief Alias for type storing angle values in radians.
/// @ingroup units
/// @see radians_
export template <typename T>
using radians_t = tagged_quantity<T, units::radian>;

/// @brief Creates a tagged quantity storing value in radians.
/// @ingroup units
/// @see degrees_
/// @see right_angles_
/// @see right_angle_
/// @see turns_
export template <typename T>
constexpr auto radians_(const T value) noexcept -> radians_t<T> {
    return radians_t<T>{value};
}

/// @brief Creates a tagged quantity in units of right angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angle_
/// @see turns_
export template <typename T>
constexpr auto right_angles_(const T value) noexcept {
    return radians_(value * units::scales::pi::_pi() / 2);
}

/// @brief Creates a tagged quantity a right angle value.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
export constexpr auto right_angle_() noexcept {
    return right_angles_(1);
}

/// @brief Creates a tagged quantity in full turn units.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angle_
/// @see right_angles_
export template <typename T>
constexpr auto turns_(const T value) noexcept {
    return radians_(value * units::scales::pi::_pi() * 2);
}

/// @brief Overload of sine for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename Qty>
constexpr auto sin(const Qty& qty)
    requires(units::is_convertible_quantity_v<Qty, units::radian>)
{
    using std::sin;
    return sin(value(convert_to<units::radian>(qty)));
}

/// @brief Overload of cosine for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename Qty>
constexpr auto cos(const Qty& qty)
    requires(units::is_convertible_quantity_v<Qty, units::radian>)
{
    using std::cos;
    return cos(value(convert_to<units::radian>(qty)));
}

/// @brief Overload of tangens for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename Qty>
constexpr auto tan(const Qty& qty)
    requires(units::is_convertible_quantity_v<Qty, units::radian>)
{
    using std::tan;
    return tan(value(convert_to<units::radian>(qty)));
}

/// @brief Overload of arc sine for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename T>
constexpr auto arcsin(const T x) {
    using std::asin;
    return units::make_tagged_quantity<units::radian>(asin(x));
}

/// @brief Overload of arc cosine for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename T>
constexpr auto arccos(const T x) {
    using std::acos;
    return units::make_tagged_quantity<units::radian>(acos(x));
}

/// @brief Overload of tangens for quantities of angle.
/// @ingroup units
/// @see radians_
/// @see degrees_
/// @see right_angles_
/// @see turns_
/// @relates tagged_quantity
export template <typename T>
constexpr auto arctan(const T y, const T x) {
    using std::atan2;
    return units::make_tagged_quantity<units::radian>(atan2(y, x));
}

/// @brief Alias for type storing angle values in degrees.
/// @ingroup units
/// @see degrees_
export template <typename T>
using degrees_t = tagged_quantity<T, units::degree>;

/// @brief Creates a tagged quantity storing value in degrees.
/// @ingroup units
/// @see radians_
/// @see right_angles_
/// @see right_angle_
/// @see turns_
export template <typename T>
constexpr auto degrees_(const T value) noexcept -> degrees_t<T> {
    return degrees_t<T>{value};
}

/// @brief Alias for type storing temperature values in kelvins.
/// @ingroup units
/// @see kelvins_
export template <typename T>
using kelvins_t = tagged_quantity<T, units::kelvin>;

/// @brief Creates a tagged quantity storing value in kelvins.
/// @ingroup units
export template <typename T>
constexpr auto kelvins_(const T value) noexcept -> kelvins_t<T> {
    return kelvins_t<T>{value};
}
//------------------------------------------------------------------------------
} // namespace eagine
