/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:quantity;

import std;
import eagine.core.types;
import :base;

namespace eagine::units {
//------------------------------------------------------------------------------
export template <typename T, typename U>
class tagged_quantity;

export template <typename U, typename T>
constexpr auto make_tagged_quantity(const T& value) -> tagged_quantity<T, U>;
//------------------------------------------------------------------------------
/// @brief Value of type T with a specified unit or tag type U.
/// @ingroup units
export template <typename T, typename U>
class tagged_quantity {
public:
    /// @brief Alias for the unit type.
    using unit_type = U;

    /// @brief Alias for the value type.
    using value_type = T;

    /// @brief Default constructor.
    tagged_quantity() = default;

    /// @brief Explicit constructor initializing the value.
    explicit constexpr tagged_quantity(T v) noexcept
      : _v{std::move(v)} {}

    /// @brief Converting constructor from another tagged quantity.
    template <typename X, typename UX>
    constexpr tagged_quantity(const tagged_quantity<X, UX>& tq) noexcept
        requires(std::is_convertible_v<X, T> and is_convertible_v<UX, U>)
      : _v{value_conv<UX, U>()(T(tq._v))} {}

    /// @brief Conversion to a quantity in another unit type.
    template <typename UX>
    constexpr auto to() const noexcept
        requires(is_convertible_v<U, UX>)
    {
        return make_tagged_quantity<UX>(value_conv<U, UX>()(_v));
    }

    /// @brief Returns the value.
    constexpr auto value() const noexcept -> T {
        return _v;
    }

    /// @brief Returns the unit.
    static constexpr auto unit() noexcept -> U {
        return {};
    }

    /// Brief Explicit conversion of the value to the specified.
    template <typename X>
    explicit constexpr operator X() const noexcept
        requires(std::is_convertible_v<T, X>)
    {
        return X(_v);
    }

    /// @brief Addition of another tagged quantity.
    template <typename X, typename UX>
    auto operator+=(const tagged_quantity<X, UX>& q) noexcept
      -> auto& requires(std::is_convertible_v<X, T>&& is_convertible_v<UX, U>) {
          _v += value_conv<UX, U>()(T(q._v));
          return *this;
      }

    /// @brief Subtraction of another tagged quantity.
    template <typename X, typename UX>
    auto operator-=(const tagged_quantity<X, UX>& q) noexcept
      -> auto& requires(std::is_convertible_v<X, T>&& is_convertible_v<UX, U>) {
          _v -= value_conv<UX, U>()(T(q._v));
          return *this;
      }

    private : static_assert(std::is_arithmetic_v<T>);
    T _v{};

    template <typename, typename>
    friend class tagged_quantity;
};
//------------------------------------------------------------------------------
template <typename U, typename T>
constexpr auto make_tagged_quantity(const T& value) -> tagged_quantity<T, U> {
    return tagged_quantity<T, U>{value};
}
//------------------------------------------------------------------------------
export template <typename T>
struct is_tagged_quantity : std::false_type {};

export template <typename T>
using is_tagged_quantity_t = typename is_tagged_quantity<T>::type;

export template <typename T>
constexpr bool is_tagged_quantity_v = is_tagged_quantity<T>::value;

export template <typename T, typename U>
struct is_tagged_quantity<tagged_quantity<T, U>> : std::true_type {};
//------------------------------------------------------------------------------
export template <typename Qty, typename Unit>
struct is_convertible_quantity : std::false_type {};

export template <typename Q, typename U>
using is_convertible_quantity_t = typename is_convertible_quantity<Q, U>::type;

export template <typename Q, typename U>
constexpr bool is_convertible_quantity_v = is_convertible_quantity<Q, U>::value;

export template <typename T, typename QU, typename U>
struct is_convertible_quantity<tagged_quantity<T, QU>, U>
  : is_convertible<QU, U> {};
//------------------------------------------------------------------------------
export template <typename T, typename U>
    requires(not is_tagged_quantity_v<T> and not is_unit_v<T> and is_unit_v<U>)
constexpr auto operator*(const T& v, U) {
    return make_tagged_quantity<U>(v);
}
//------------------------------------------------------------------------------
export template <typename UX, typename T, typename U>
constexpr auto convert_to(const tagged_quantity<T, U>& q) {
    return q.template to<UX>();
}

export template <typename T, typename U>
constexpr auto value(const tagged_quantity<T, U>& q) {
    return q.value();
}
//------------------------------------------------------------------------------
/// @brief Equality comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator==(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) == value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Nonequality comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator!=(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) != value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Less-than comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator<(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) < value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Less-equal comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator<=(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) <= value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Greater-than comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator>(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) > value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Greater-equal comparison.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
    requires(is_convertible_v<U2, U1>)
constexpr auto operator>=(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) -> bool {
    return value(a) >= value_conv<U2, U1>()(value(b));
}
//------------------------------------------------------------------------------
/// @brief Unary plus operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T, typename U>
constexpr auto operator+(const tagged_quantity<T, U>& a) {
    return a;
}
//------------------------------------------------------------------------------
/// @brief Unary negation operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T, typename U>
constexpr auto operator-(const tagged_quantity<T, U>& a) {
    return make_tagged_quantity<U>(-value(a));
}
//------------------------------------------------------------------------------
/// @brief Addition operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
constexpr auto operator+(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) {
    using UR = add_result_t<U1, U2>;
    return make_tagged_quantity<UR>(
      value_conv<U1, UR>()(value(a)) + value_conv<U2, UR>()(value(b)));
}
//------------------------------------------------------------------------------
/// @brief Subtraction operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
constexpr auto operator-(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) {
    using UR = sub_result_t<U1, U2>;
    return make_tagged_quantity<UR>(
      value_conv<U1, UR>()(value(a)) - value_conv<U2, UR>()(value(b)));
}
//------------------------------------------------------------------------------
/// @brief Multiplication operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
constexpr auto operator*(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) {
    using UO1 = mul_l_operand_t<U1, U2>;
    using UO2 = mul_r_operand_t<U1, U2>;
    using UR = mul_result_t<U1, U2>;
    return make_tagged_quantity<UR>(
      value_conv<U1, UO1>()(value(a)) * value_conv<U2, UO2>()(value(b)));
}
//------------------------------------------------------------------------------
/// @brief Multiplication by constant operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U, typename T2>
    requires(not is_unit_v<T2> and not is_tagged_quantity_v<T2>)
constexpr auto operator*(const tagged_quantity<T1, U>& a, const T2& c) {
    return make_tagged_quantity<U>(value(a) * c);
}
//------------------------------------------------------------------------------
/// @brief Multiplication by constant operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename T2, typename U>
constexpr auto operator*(const T1& c, const tagged_quantity<T2, U>& a) {
    return make_tagged_quantity<U>(c * value(a));
}
//------------------------------------------------------------------------------
/// @brief Multiplication by unit operator.
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename U2>
    requires(is_unit_v<U2>)
constexpr auto operator*(const tagged_quantity<T1, U1>& a, U2) {
    return a * make_tagged_quantity<U2>(1);
}
//------------------------------------------------------------------------------
/// @brief Division operator
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename T2, typename U2>
constexpr auto operator/(
  const tagged_quantity<T1, U1>& a,
  const tagged_quantity<T2, U2>& b) {
    using UO1 = div_l_operand_t<U1, U2>;
    using UO2 = div_r_operand_t<U1, U2>;
    using UR = div_result_t<U1, U2>;
    return make_tagged_quantity<UR>(
      value_conv<U1, UO1>()(value(a)) / value_conv<U2, UO2>()(value(b)));
}
//------------------------------------------------------------------------------
/// @brief Division by constant operator
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U, typename T2>
    requires(not is_unit_v<T2>)
constexpr auto operator/(const tagged_quantity<T1, U>& a, const T2& c) {
    return make_tagged_quantity<U>((1.F * value(a)) / c);
}
//------------------------------------------------------------------------------
/// @brief Constant division operator
/// @ingroup units
/// @relates tagged_quantity
export template <typename T1, typename U1, typename U2>
    requires(is_unit_v<U2>)
constexpr auto operator/(const tagged_quantity<T1, U1>& a, U2) {
    return a / make_tagged_quantity<U2>(1);
}
//------------------------------------------------------------------------------
}; // namespace eagine::units
