/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:matrix_construct;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.vectorization;
import eagine.core.units;
import :traits;
import :vector;
import :matrix;
import <optional>;
import <utility>;

namespace eagine {
namespace math {
//------------------------------------------------------------------------------
export template <template <class> class MC, typename T, int C, int R, bool RM, bool V>
struct constructed_matrix<MC<matrix<T, C, R, RM, V>>>
  : std::conditional_t<
      is_matrix_constructor_v<MC<matrix<T, C, R, RM, V>>>,
      matrix<T, C, R, RM, V>,
      nothing_t> {};

export template <
  template <class, int>
  class MC,
  typename T,
  int C,
  int R,
  bool RM,
  bool V,
  int I>
struct constructed_matrix<MC<matrix<T, C, R, RM, V>, I>>
  : std::conditional_t<
      is_matrix_constructor_v<MC<matrix<T, C, R, RM, V>, I>>,
      matrix<T, C, R, RM, V>,
      nothing_t> {};

/// @brief Uses the specified matrix constructor @p c to construct a matrix.
/// @ingroup math
export template <bool RM, typename MC>
[[nodiscard]] constexpr auto construct_matrix(const MC& c) noexcept
  -> constructed_matrix_t<MC>
    requires(
      is_matrix_constructor_v<MC> &&
      is_row_major_v<constructed_matrix_t<MC>> == RM)
{
    return c();
}

// construct_matrix (reorder)
export template <bool RM, typename MC>
[[nodiscard]] constexpr auto construct_matrix(const MC& c) noexcept
  -> reordered_matrix_t<constructed_matrix_t<MC>>
    requires(
      is_matrix_constructor_v<MC> &&
      is_row_major_v<constructed_matrix_t<MC>> != RM)
{
    return reorder_mat_ctr(c)();
}

/// @brief Multiplies the results of two matrix constructors.
/// @ingroup math
///
/// This is typically more efficient than constructing the two matrices and
/// multiplying them.
export template <typename MC1, typename MC2>
[[nodiscard]] constexpr auto multiply(const MC1& mc1, const MC2& mc2) noexcept
    requires(
      is_matrix_constructor_v<MC1> && is_matrix_constructor_v<MC2> &&
      are_multiplicable<constructed_matrix_t<MC1>, constructed_matrix_t<MC2>>::
        value)
{
    return multiply(construct_matrix<true>(mc1), construct_matrix<false>(mc2));
}

/// @brief Helper class used in matrix constructor implementation.
/// @ingroup math
export template <typename MC>
class convertible_matrix_constructor : public MC {
    static_assert(is_matrix_constructor_v<MC>);

public:
    /// @brief Forwards arguments to the basic matrix constructor.
    template <typename... P>
    convertible_matrix_constructor(P&&... p)
      : MC(std::forward<P>(p)...) {}

    /// @brief Implicit conversion to the constructed matrix type.
    operator constructed_matrix_t<MC>() const noexcept {
        return MC::operator()();
    }
};

export template <typename MC>
struct is_matrix_constructor<convertible_matrix_constructor<MC>>
  : is_matrix_constructor<MC> {};

export template <typename MC>
struct constructed_matrix<convertible_matrix_constructor<MC>>
  : constructed_matrix<MC> {};
//------------------------------------------------------------------------------
export template <typename X>
class identity;

// is_matrix_constructor<identity>
export template <typename T, int R, int C, bool RM, bool V>
struct is_matrix_constructor<identity<matrix<T, R, C, RM, V>>>
  : std::true_type {};

/// @brief Identity matrix constructor.
/// @ingroup math
/// @see is_matrix_constructor_v
/// @see constructed_matrix_t
export template <typename T, int R, int C, bool RM, bool V>
class identity<matrix<T, R, C, RM, V>> {
public:
    /// @brief Returns an identity matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept
      -> matrix<T, R, C, RM, V> {
        using _riS = _make_useq<RM ? R : C>;
        return _identity(_riS());
    }

private:
    template <int... U>
    using _useq = std::integer_sequence<int, U...>;

    template <int N>
    using _make_useq = std::make_integer_sequence<int, N>;

    template <int... I>
    static constexpr auto _identity(const _useq<I...>) noexcept
      -> matrix<T, R, C, RM, V> {
        return {{vect::axis < T, RM ? C : R, I, V > ::apply(1)...}};
    }
};
//------------------------------------------------------------------------------
/// @brief Returns the inverse matrix to the matrix passed as argument.
/// @ingroup math
export template <typename T, int N, bool RM, bool V>
[[nodiscard]] constexpr auto inverse_matrix(matrix<T, N, N, RM, V> m) noexcept
  -> std::optional<matrix<T, N, N, RM, V>> {
    auto i = identity<matrix<T, N, N, RM, V>>()();
    if(gauss_jordan_elimination(m, i)) {
        return {i};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns the inverse matrix to the matrix constructed by the argument.
/// @ingroup math
export template <typename Ctr>
[[nodiscard]] constexpr auto inverse_matrix(const Ctr& ctr) noexcept
    requires(is_matrix_constructor_v<Ctr>)
{
    // TODO: reorder to row major?
    return inverse_matrix(ctr());
}
//------------------------------------------------------------------------------
// translation
export template <typename X>
class translation;

// is_matrix_constructor<translation>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<translation<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of translation transformation matrix.
/// @ingroup math
/// @see matrix_translation
///
/// @note Do not use directly, use matrix_translation
export template <typename T, bool RM, bool V>
class translation<matrix<T, 4, 4, RM, V>> {
public:
    constexpr translation(const vect::data_t<T, 3, V> v) noexcept
      : _v{v} {}

    /// @brief Initializes the matrix constructor.
    /// @param vx is the x-axis translation distance.
    /// @param vy is the y-axis translation distance.
    /// @param vz is the z-axis translation distance.
    constexpr translation(const T vx, const T vy, const T vz) noexcept
      : _v{vx, vy, vz} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return _make(std::bool_constant<RM>());
    }

    [[nodiscard]] friend constexpr auto reorder_mat_ctr(
      const translation<matrix<T, 4, 4, RM, V>>& c) noexcept
      -> translation<matrix<T, 4, 4, !RM, V>> {
        return {c._v};
    }

private:
    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{T(1), T(0), T(0), _v[0]},
           {T(0), T(1), T(0), _v[1]},
           {T(0), T(0), T(1), _v[2]},
           {T(0), T(0), T(0), T(1)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return matrix<T, 4, 4, false, V>{
          {{T(1), T(0), T(0), T(0)},
           {T(0), T(1), T(0), T(0)},
           {T(0), T(0), T(1), T(0)},
           {_v[0], _v[1], _v[2], T(1)}}};
    }

    vect::data_t<T, 3, V> _v;
};

// multiply
export template <typename T, int N, bool RM1, bool RM2, bool V>
constexpr auto multiply(
  const translation<matrix<T, N, N, RM1, V>>& a,
  const translation<matrix<T, N, N, RM2, V>>& b) noexcept
  -> translation<matrix<T, N, N, RM1, V>> {
    return {a._v + b._v};
}

/// @brief Alias for constructor of uniform translation transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_reflection_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_translation =
  convertible_matrix_constructor<translation<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// scale
export template <typename X>
class scale;

// is_matrix_constructor<scale>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<scale<matrix<T, N, N, RM, V>>> : std::true_type {};

/// @brief Implements constructor of scale transformation matrix.
/// @ingroup math
/// @see matrix_scale
///
/// @note Do not use directly, use matrix_scale
export template <typename T, bool RM, bool V>
class scale<matrix<T, 4, 4, RM, V>> {
public:
    constexpr scale(const vect::data_t<T, 3, V> v) noexcept
      : _v{v} {}

    /// @brief Initializes the matrix constructor.
    /// @param vx is the x-axis scale factor.
    /// @param vy is the y-axis scale factor.
    /// @param vz is the z-axis scale factor.
    constexpr scale(const T vx, const T vy, const T vz) noexcept
      : _v{vx, vy, vz} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{_v[0], T(0), T(0), T(0)},
           {T(0), _v[1], T(0), T(0)},
           {T(0), T(0), _v[2], T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

    [[nodiscard]] friend constexpr auto reorder_mat_ctr(
      const scale<matrix<T, 4, 4, RM, V>>& c) noexcept
      -> scale<matrix<T, 4, 4, !RM, V>> {
        return {c._v};
    }

private:
    vect::data_t<T, 3, V> _v;
};

// multiply
export template <typename T, int N, bool RM1, bool RM2, bool V>
[[nodiscard]] constexpr auto multiply(
  const scale<matrix<T, N, N, RM1, V>>& a,
  const scale<matrix<T, N, N, RM2, V>>& b) noexcept
  -> scale<matrix<T, N, N, RM1, V>> {
    return {a._v * b._v};
}

// uniform_scale
export template <typename X>
class uniform_scale;

// is_matrix_constructor<uniform_scale>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<uniform_scale<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of uniform scale transformation matrix.
/// @ingroup math
/// @see matrix_uniform_scale
///
/// @note Do not use directly, use matrix_uniform_scale
export template <typename T, bool RM, bool V>
class uniform_scale<matrix<T, 4, 4, RM, V>> {
public:
    /// @brief Initializes the matrix constructor.
    /// @param v is the scale factor.
    constexpr uniform_scale(T v) noexcept
      : _v(v) {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{_v, T(0), T(0), T(0)},
           {T(0), _v, T(0), T(0)},
           {T(0), T(0), _v, T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

    [[nodiscard]] friend constexpr auto reorder_mat_ctr(
      const uniform_scale<matrix<T, 4, 4, RM, V>>& c) noexcept
      -> uniform_scale<matrix<T, 4, 4, !RM, V>> {
        return {c._v};
    }

private:
    T _v;
};

// multiply
export template <typename T, int N, bool RM1, bool RM2, bool V>
[[nodiscard]] constexpr auto multiply(
  const uniform_scale<matrix<T, N, N, RM1, V>>& a,
  const uniform_scale<matrix<T, N, N, RM2, V>>& b) noexcept
  -> uniform_scale<matrix<T, N, N, RM1, V>> {
    return {a._v * b._v};
}

/// @brief Alias for constructor of scale transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_scale =
  convertible_matrix_constructor<scale<matrix<T, 4, 4, true, V>>>;

/// @brief Alias for constructor of uniform scale transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_reflection_z
/// @see matrix_scale
export template <typename T, bool V>
using matrix_uniform_scale =
  convertible_matrix_constructor<uniform_scale<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// reflection_I
export template <typename X, int I>
class reflection_I;

// is_matrix_constructor<reflection_I>
export template <typename T, int N, bool RM, bool V, int I>
struct is_matrix_constructor<reflection_I<matrix<T, N, N, RM, V>, I>>
  : std::true_type {};

/// @brief Implements constructor of reflection transformation matrix.
/// @ingroup math
/// @tparam I index of the reflection axis (0 = x, 1 = y, 2 = z).
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_reflection_z
///
/// @note Do not use directly, use matrix_reflection_{x,y,z}
export template <typename T, bool RM, bool V, int I>
class reflection_I<matrix<T, 4, 4, RM, V>, I> {
public:
    /// @brief Initializes the matrix constructor.
    /// @param r says whether to actually reflect along the I axis.
    constexpr reflection_I(const bool r = true) noexcept
      : _v{r ? T(-1) : T(1)} {}

    [[nodiscard]] constexpr auto v(const int i) const noexcept -> T {
        return (I == i) ? _v : T(1);
    }

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{v(0), T(0), T(0), T(0)},
           {T(0), v(1), T(0), T(0)},
           {T(0), T(0), v(2), T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

private:
    T _v{};
};

// multiply
export template <typename T, int N, bool RM1, bool RM2, bool V, int I>
[[nodiscard]] constexpr auto multiply(
  const reflection_I<matrix<T, N, N, RM1, V>, I>& a,
  const reflection_I<matrix<T, N, N, RM2, V>, I>& b) noexcept
  -> reflection_I<matrix<T, N, N, RM1, V>, I> {
    return {(a._v < b._v) || (a._v > b._v)};
}

// reorder_mat_ctr(reflection_I)
export template <typename T, int N, bool RM, bool V, int I>
[[nodiscard]] constexpr auto reorder_mat_ctr(
  const reflection_I<matrix<T, N, N, RM, V>, I>& c) noexcept
  -> reflection_I<matrix<T, N, N, !RM, V>, I> {
    return {c._v < T(0)};
}

/// @brief Alias for implementation of constructor of reflection along x-axis matrix.
/// @ingroup math
export template <typename M>
using reflection_x = reflection_I<M, 0>;

/// @brief Alias for implementation of constructor of reflection along y-axis matrix.
/// @ingroup math
export template <typename M>
using reflection_y = reflection_I<M, 1>;

/// @brief Alias for implementation of constructor of reflection along z-axis matrix.
/// @ingroup math
export template <typename M>
using reflection_z = reflection_I<M, 2>;

/// @brief Alias for constructor of reflection along x-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_reflection_y
/// @see matrix_reflection_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_reflection_x =
  convertible_matrix_constructor<reflection_x<matrix<T, 4, 4, true, V>>>;

/// @brief Alias for constructor of reflection along y-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_reflection_x
/// @see matrix_reflection_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_reflection_y =
  convertible_matrix_constructor<reflection_y<matrix<T, 4, 4, true, V>>>;

/// @brief Alias for constructor of reflection along z-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_reflection_z =
  convertible_matrix_constructor<reflection_z<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// rotation_I
export template <typename X, int I>
class rotation_I;

// is_matrix_constructor<rotation_I>
export template <typename T, int N, bool RM, bool V, int I>
struct is_matrix_constructor<rotation_I<matrix<T, N, N, RM, V>, I>>
  : std::true_type {};

/// @brief Implements constructor of rotation transformation matrix.
/// @ingroup math
/// @tparam I index of the rotation axis (0 = x, 1 = y, 2 = z).
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_rotation_z
///
/// @note Do not use directly, use matrix_rotation_{x,y,z}
export template <typename T, bool RM, bool V, int I>
class rotation_I<matrix<T, 4, 4, RM, V>, I> {
public:
    /// @brief Initializes the matrix constructor.
    /// @param v is the angle of rotation.
    constexpr rotation_I(const radians_t<T> v) noexcept
      : _v{v} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const {
        using _axis = int_constant<I>;
        return _make(cos(_v), sin(_v) * (RM ? 1 : -1), _axis());
    }

    [[nodiscard]] friend constexpr auto reorder_mat_ctr(
      const rotation_I<matrix<T, 4, 4, RM, V>, I>& c) noexcept
      -> rotation_I<matrix<T, 4, 4, !RM, V>, I> {
        return {c._v};
    }

private:
    using _x = int_constant<0>;
    using _y = int_constant<1>;
    using _z = int_constant<2>;

    static constexpr auto _make(const T cx, const T sx, _x) noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{T(1), T(0), T(0), T(0)},
           {T(0), cx, -sx, T(0)},
           {T(0), sx, cx, T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

    static constexpr auto _make(const T cx, const T sx, _y) noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{cx, T(0), sx, T(0)},
           {T(0), T(1), T(0), T(0)},
           {-sx, T(0), cx, T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

    static constexpr auto _make(const T cx, const T sx, _z) noexcept {
        return matrix<T, 4, 4, RM, V>{
          {{cx, -sx, T(0), T(0)},
           {sx, cx, T(0), T(0)},
           {T(0), T(0), T(1), T(0)},
           {T(0), T(0), T(0), T(1)}}};
    }

    radians_t<T> _v;
};

// multiply
export template <typename T, int N, bool RM1, bool RM2, bool V, int I>
[[nodiscard]] constexpr auto multiply(
  const rotation_I<matrix<T, N, N, RM1, V>, I>& a,
  const rotation_I<matrix<T, N, N, RM2, V>, I>& b) noexcept
  -> rotation_I<matrix<T, N, N, RM1, V>, I> {
    return {radians_t<T>{T(a._v) + T(b._v)}};
}

/// @brief Alias for implementation of constructor of rotation along x-axis matrix.
/// @ingroup math
export template <typename M>
using rotation_x = rotation_I<M, 0>;

/// @brief Alias for implementation of constructor of rotation along y-axis matrix.
/// @ingroup math
export template <typename M>
using rotation_y = rotation_I<M, 1>;

/// @brief Alias for implementation of constructor of rotation along z-axis matrix.
/// @ingroup math
export template <typename M>
using rotation_z = rotation_I<M, 2>;

/// @brief Alias for constructor of rotation along x-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_y
/// @see matrix_rotation_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_rotation_x =
  convertible_matrix_constructor<rotation_x<matrix<T, 4, 4, true, V>>>;

/// @brief Alias for constructor of rotation along y-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_z
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_reflection_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_rotation_y =
  convertible_matrix_constructor<rotation_y<matrix<T, 4, 4, true, V>>>;

/// @brief Alias for constructor of rotation along z-axis transformation matrix.
/// @ingroup math
/// @see matrix_translation
/// @see matrix_rotation_x
/// @see matrix_rotation_y
/// @see matrix_reflection_x
/// @see matrix_reflection_y
/// @see matrix_reflection_z
/// @see matrix_scale
/// @see matrix_uniform_scale
export template <typename T, bool V>
using matrix_rotation_z =
  convertible_matrix_constructor<rotation_z<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// ortho
export template <typename X>
class ortho;

// is_matrix_constructor<ortho>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<ortho<matrix<T, N, N, RM, V>>> : std::true_type {};

/// @brief Implements constructor of orthographic projection matrix.
/// @ingroup math
/// @see matrix_ortho
///
/// @note Do not use directly, use matrix_ortho.
export template <typename T, bool RM, bool V>
class ortho<matrix<T, 4, 4, RM, V>> {
public:
    constexpr ortho(const vect::data_t<T, 6, V>& v) noexcept
      : _v{v} {}

    /// @brief Initializes the matrix constructor.
    constexpr ortho(
      const T x_left,
      const T x_right,
      const T y_bottom,
      const T y_top,
      const T z_near,
      const T z_far) noexcept
      : _v{x_left, x_right, y_bottom, y_top, z_near, z_far} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return _make(std::bool_constant<RM>());
    }

private:
    constexpr auto _x_left() const {
        return _v[0];
    }
    constexpr auto _x_right() const {
        return _v[1];
    }
    constexpr auto _y_bottom() const {
        return _v[2];
    }
    constexpr auto _y_top() const {
        return _v[3];
    }
    constexpr auto _z_near() const {
        return _v[4];
    }
    constexpr auto _z_far() const {
        return _v[5];
    }

    constexpr auto _m00() const noexcept {
        return T(2) / (_x_right() - _x_left());
    }

    constexpr auto _m11() const noexcept {
        return T(2) / (_y_top() - _y_bottom());
    }

    constexpr auto _m22() const noexcept {
        return -T(2) / (_z_far() - _z_near());
    }

    constexpr auto _m30() const noexcept {
        return -(_x_right() + _x_left()) / (_x_right() - _x_left());
    }

    constexpr auto _m31() const noexcept {
        return -(_y_top() + _y_bottom()) / (_y_top() - _y_bottom());
    }

    constexpr auto _m32() const noexcept {
        return -(_z_far() + _z_near()) / (_z_far() - _z_near());
    }

    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{_m00(), T(0), T(0), _m30()},
           {T(0), _m11(), T(0), _m31()},
           {T(0), T(0), _m22(), _m32()},
           {T(0), T(0), T(0), T(1)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return matrix<T, 4, 4, false, V>{
          {{_m00(), T(0), T(0), T(0)},
           {T(0), _m11(), T(0), T(0)},
           {T(0), T(0), _m22(), T(0)},
           {_m30(), _m31(), _m32(), T(1)}}};
    }

    using _dT = vect::data_t<T, 6, V>;
    _dT _v;
};

// reorder_mat_ctr(ortho)
export template <typename T, int N, bool RM, bool V>
[[nodiscard]] constexpr auto reorder_mat_ctr(
  const ortho<matrix<T, N, N, RM, V>>& c) noexcept
  -> ortho<matrix<T, N, N, !RM, V>> {
    return {c._v};
}

/// @brief Alias for constructor of orthographic projection matrix.
/// @ingroup math
export template <typename T, bool V>
using matrix_ortho =
  convertible_matrix_constructor<ortho<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// perspective
export template <typename X>
class perspective;

// is_matrix_constructor<perspective>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<perspective<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of perspective projection matrix.
/// @ingroup math
/// @see matrix_perspective
///
/// @note Do not use directly, use matrix_perspective.
export template <typename T, bool RM, bool V>
class perspective<matrix<T, 4, 4, RM, V>> {
public:
    constexpr perspective(const vect::data_t<T, 6, V>& v) noexcept
      : _v{v} {}

    /// @brief Initialized the matrix constructor.
    constexpr perspective(
      const T x_left,
      const T x_right,
      const T y_bottom,
      const T y_top,
      const T z_near,
      const T z_far) noexcept
      : _v{x_left, x_right, y_bottom, y_top, z_near, z_far} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return _make(std::bool_constant<RM>());
    }

    /// @brief Constructs perspective matrix with x-FOV angle and aspect ratio.
    [[nodiscard]] static auto x(
      const radians_t<T> xfov,
      const T aspect,
      const T z_near,
      const T z_far) noexcept {
        assert(aspect > T(0));
        assert(T(xfov) > T(0));

        const T x_right = z_near * tan(xfov * T(0.5));
        const T x_left = -x_right;

        const T y_bottom = x_left / aspect;
        const T y_top = x_right / aspect;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

    /// @brief Constructs perspective matrix with y-FOV angle and aspect ratio.
    [[nodiscard]] static auto y(
      const radians_t<T> yfov,
      const T aspect,
      const T z_near,
      const T z_far) noexcept {
        assert(aspect > T(0));
        assert(T(yfov) > T(0));

        const T y_top = z_near * tan(yfov * T(0.5));
        const T y_bottom = -y_top;

        const T x_left = y_bottom * aspect;
        const T x_right = y_top * aspect;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

    /// @brief Constructs perspective matrix with FOV angle and aspect ratio of 1.
    [[nodiscard]] static auto square(
      const radians_t<T> fov,
      const T z_near,
      const T z_far) noexcept {
        assert(T(fov) > T(0));

        const T x_right = z_near * tan(fov * T(0.5));
        const T x_left = -x_right;

        const T y_bottom = x_left;
        const T y_top = x_right;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

private:
    constexpr auto _x_left() const noexcept {
        return _v[0];
    }

    constexpr auto _x_right() const noexcept {
        return _v[1];
    }

    constexpr auto _y_bottom() const noexcept {
        return _v[2];
    }

    constexpr auto _y_top() const noexcept {
        return _v[3];
    }

    constexpr auto _z_near() const noexcept {
        return _v[4];
    }

    constexpr auto _z_far() const noexcept {
        return _v[5];
    }

    constexpr auto _m00() const noexcept {
        return (T(2) * _z_near()) / (_x_right() - _x_left());
    }

    constexpr auto _m11() const noexcept {
        return (T(2) * _z_near()) / (_y_top() - _y_bottom());
    }

    constexpr auto _m22() const noexcept {
        return -(_z_far() + _z_near()) / (_z_far() - _z_near());
    }

    constexpr auto _m20() const noexcept {
        return (_x_right() + _x_left()) / (_x_right() - _x_left());
    }

    constexpr auto _m21() const noexcept {
        return (_y_top() + _y_bottom()) / (_y_top() - _y_bottom());
    }

    constexpr auto _m23() const noexcept {
        return -T(1);
    }

    constexpr auto _m32() const noexcept {
        return -(T(2) * _z_far() * _z_near()) / (_z_far() - _z_near());
    }

    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{_m00(), T(0), _m20(), T(0)},
           {T(0), _m11(), _m21(), T(0)},
           {T(0), T(0), _m22(), _m32()},
           {T(0), T(0), _m23(), T(0)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return matrix<T, 4, 4, false, V>{
          {{_m00(), T(0), T(0), T(0)},
           {T(0), _m11(), T(0), T(0)},
           {_m20(), _m21(), _m22(), _m23()},
           {T(0), T(0), _m32(), T(0)}}};
    }

    using _dT = vect::data_t<T, 6, V>;
    _dT _v;
};

// reorder_mat_ctr(perspective)
export template <typename T, int N, bool RM, bool V>
[[nodiscard]] constexpr auto reorder_mat_ctr(
  const perspective<matrix<T, N, N, RM, V>>& c) noexcept
  -> perspective<matrix<T, N, N, !RM, V>> {
    return {c._v};
}

/// @brief Alias for constructor of perspective projection matrix.
/// @ingroup math
export template <typename T, bool V>
using matrix_perspective =
  convertible_matrix_constructor<perspective<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// looking_at_y_up
export template <typename X>
class looking_at_y_up;

// is_matrix_constructor<looking_at_y_up>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<looking_at_y_up<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of look-at matrix used for camera transformations.
/// @ingroup math
/// @see matrix_looking_at_y_up
///
/// This implementation assumes the positive y axis points up.
/// @note Do not use directly, use matrix_looking_at_y_up
export template <typename T, bool RM, bool V>
class looking_at_y_up<matrix<T, 4, 4, RM, V>> {
public:
    /// @brief Initializes the matrix constructor.
    /// @param eye is the position of the camera.
    /// @param target is the point the camera is looking at.
    constexpr looking_at_y_up(
      const vector<T, 3, V>& eye,
      const vector<T, 3, V>& target) noexcept
      : _e(eye)
      , _t(target) {}

    /// @brief Returns the constructed matrix.
    constexpr auto operator()() const noexcept {
        return _make(std::bool_constant<RM>());
    }

private:
    using _dT = vector<T, 3, V>;

    vector<T, 3, V> _e, _t;

    static constexpr auto _make(
      const _dT& x,
      const _dT& y,
      const _dT& z,
      const _dT& t) noexcept {
        return matrix<T, 4, 4, true, V>{
          {{x[0], x[1], x[2], -dot(x, t)},
           {y[0], y[1], y[2], -dot(y, t)},
           {z[0], z[1], z[2], -dot(z, t)},
           {T(0), T(0), T(0), T(1)}}};
    }

    static constexpr auto _make(
      const _dT& y,
      const _dT& z,
      const _dT& t) noexcept {
        return _make(cross(y, z), y, z, t);
    }

    static constexpr auto _make(const _dT& z, const _dT& t) noexcept {
        return _make(normalized(cross(z, t)), z, t);
    }

    static constexpr auto _make(const _dT& z) noexcept {
        return _make(z, _dT::make(z.z(), T(0), -z.x()));
    }

    constexpr auto _make(std::true_type) const noexcept {
        return _make(normalized(_e - _t));
    }

    constexpr auto _make(std::false_type) const noexcept {
        return reorder(_make(std::true_type()));
    }
};

// reorder_mat_ctr(looking_at_y_up)
export template <typename T, int N, bool RM, bool V>
[[nodiscard]] constexpr auto reorder_mat_ctr(
  const looking_at_y_up<matrix<T, N, N, RM, V>>& c) noexcept
  -> looking_at_y_up<matrix<T, N, N, !RM, V>> {
    return {c._e, c._t};
}

/// @brief Alias for constructor of look-at matrix used for camera transformations.
/// @ingroup math
export template <typename T, bool V>
using matrix_looking_at_y_up =
  convertible_matrix_constructor<looking_at_y_up<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
// orbiting_y_up
export template <typename X>
class orbiting_y_up;

// is_matrix_constructor<orbiting_y_up>
export template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<orbiting_y_up<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of orbiting matrix used for camera transformations.
/// @ingroup math
/// @see matrix_orbiting_y_up
///
/// This implementation assumes the positive y axis points up.
///
/// This implementation assumes the positive y axis points up.
/// @note Do not use directly, use matrix_orbiting_y_up
export template <typename T, bool RM, bool V>
class orbiting_y_up<matrix<T, 4, 4, RM, V>> {
public:
    constexpr orbiting_y_up(
      const vector<T, 3, V>& t,
      const vector<T, 3, V>& x,
      const vector<T, 3, V>& y,
      const vector<T, 3, V>& z,
      T r) noexcept
      : _t{t}
      , _x{x}
      , _z{z}
      , _y{y}
      , _r{r} {}

    constexpr orbiting_y_up(
      const vector<T, 3, V>& t,
      const T rs,
      const T sa,
      const T ca,
      const T se,
      const T ce) noexcept
      : _t{t}
      , _x{{-sa, T(0), -ca}}
      , _z{{ce * ca, se, ce * -sa}}
      , _y{cross(_z, _x)}
      , _r{rs} {}

    /// @brief Initalizes the matrix constructor.
    /// @param target is the point that the camera orbits around.
    /// @param radius is the distance from the target that the camera orbits at.
    /// @param azimuth is the azimuth (longitude) angle.
    /// @param elevation is the elevation (latitude) angle.
    constexpr orbiting_y_up(
      const vector<T, 3, V>& target,
      const T radius,
      const radians_t<T> azimuth,
      const radians_t<T> elevation)
      : orbiting_y_up{
          target,
          radius,
          sin(azimuth),
          cos(azimuth),
          sin(elevation),
          cos(elevation)} {}

    /// @brief Returns the constructed matrix.
    [[nodiscard]] constexpr auto operator()() const noexcept {
        return _make(std::bool_constant<RM>());
    }

    [[nodiscard]] friend constexpr auto reorder_mat_ctr(
      const orbiting_y_up<matrix<T, 4, 4, RM, V>>& c) noexcept
      -> orbiting_y_up<matrix<T, 4, 4, !RM, V>> {
        return {c._t, c._x, c._y, c._z, c._r};
    }

private:
    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{_x[0], _x[1], _x[2], -_r * dot(_x, _z) - dot(_x, _t)},
           {_y[0], _y[1], _y[2], -_r * dot(_y, _z) - dot(_y, _t)},
           {_z[0], _z[1], _z[2], -_r * dot(_z, _z) - dot(_z, _t)},
           {T(0), T(0), T(0), T(1)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return reorder(_make(std::true_type()));
    }

    vector<T, 3, V> _t;
    vector<T, 3, V> _x, _z, _y;
    T _r;
};

/// @brief Alias for constructor of orbiting matrix used for camera transformations.
/// @ingroup math
export template <typename T, bool V>
using matrix_orbiting_y_up =
  convertible_matrix_constructor<orbiting_y_up<matrix<T, 4, 4, true, V>>>;
//------------------------------------------------------------------------------
} // namespace math

export template <typename MC>
struct is_known_matrix_type<math::convertible_matrix_constructor<MC>>
  : is_known_matrix_type<math::constructed_matrix_t<MC>> {};

export template <typename MC>
struct canonical_compound_type<math::convertible_matrix_constructor<MC>>
  : canonical_compound_type<math::constructed_matrix_t<MC>> {};

export template <typename MC>
struct compound_view_maker<math::convertible_matrix_constructor<MC>> {
    struct _result_type {
        using M = math::constructed_matrix_t<MC>;
        using T = typename M::element_type;
        M _m;

        operator span<const T>() const noexcept {
            compound_view_maker<M> cvm;
            return cvm(_m);
        }
    };

    auto operator()(
      const math::convertible_matrix_constructor<MC>& mc) const noexcept {
        return _result_type{mc()};
    }
};

export template <typename MC>
struct is_row_major<math::convertible_matrix_constructor<MC>>
  : math::is_row_major<math::constructed_matrix_t<MC>> {};
//------------------------------------------------------------------------------
} // namespace eagine
