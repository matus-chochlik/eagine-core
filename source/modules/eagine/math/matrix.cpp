/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:matrix;

import std;
import eagine.core.types;
import eagine.core.simd;

import :traits;
import :vector;

namespace eagine {
namespace math {
//------------------------------------------------------------------------------
export template <typename X>
struct is_row_major;

export template <typename T1, typename T2>
struct are_multiplicable : std::false_type {};

export template <typename T1, typename T2>
struct multiplication_result;

/// @brief Basic RxC matrix implementation template.
/// @ingroup math
/// @see tmat
/// @note This is a base class, typically tmat should be used in client code.
export template <typename T, int C, int R, bool RM, bool V>
class matrix {

    template <int... U>
    using _iseq = std::integer_sequence<int, U...>;

    template <int N>
    using _make_iseq = std::make_integer_sequence<int, N>;

    template <typename P, int... I>
    static constexpr auto _from_hlp(
      const P* dt,
      span_size_t sz,
      _iseq<I...>) noexcept -> matrix {
        return matrix{
          {simd::from_array < T,
           RM ? C : R,
           V > ::apply(dt + I * (RM ? C : R), sz - I * (RM ? C : R))...}};
    }

    template <typename P, int M, int N, bool W, int... I>
    static constexpr auto _from_hlp(
      const matrix<P, M, N, RM, W>& m,
      _iseq<I...>) noexcept -> matrix {
        return matrix{
          {simd::cast<P, (RM ? M : N), W, T, (RM ? C : R), V>::apply(
            m._v[I], T(0))...}};
    }

    template <bool DstRM>
    static constexpr auto _transpose_tpl_hlp_4x4(
      const simd::data_t<T, 4, V>& q0,
      const simd::data_t<T, 4, V>& q1,
      const simd::data_t<T, 4, V>& q2,
      const simd::data_t<T, 4, V>& q3) noexcept {
        using simd::shuffle2;
        using simd::shuffle_mask;
        return matrix<T, 4, 4, DstRM, V>{
          {shuffle2<T, 4, V>::apply(q0, q2, shuffle_mask<0, 2, 4, 6>{}),
           shuffle2<T, 4, V>::apply(q0, q2, shuffle_mask<1, 3, 5, 7>{}),
           shuffle2<T, 4, V>::apply(q1, q3, shuffle_mask<0, 2, 4, 6>{}),
           shuffle2<T, 4, V>::apply(q1, q3, shuffle_mask<1, 3, 5, 7>{})}};
    }

    template <bool DstRM>
    constexpr auto _transpose_tpl() const noexcept -> matrix<T, 4, 4, DstRM, V>
        requires(R == 4 and C == 4)
    {
        using simd::shuffle2;
        using simd::shuffle_mask;
        return _transpose_tpl_hlp_4x4<DstRM>(
          shuffle2<T, 4, V>::apply(_v[0], _v[1], shuffle_mask<0, 1, 4, 5>{}),
          shuffle2<T, 4, V>::apply(_v[0], _v[1], shuffle_mask<2, 3, 6, 7>{}),
          shuffle2<T, 4, V>::apply(_v[2], _v[3], shuffle_mask<0, 1, 4, 5>{}),
          shuffle2<T, 4, V>::apply(_v[2], _v[3], shuffle_mask<2, 3, 6, 7>{}));
    }

    template <bool DstRM>
    constexpr auto _transpose_tpl() const noexcept
      -> matrix<T, (DstRM == RM ? R : C), (DstRM == RM ? C : R), DstRM, V> {
        const bool S = (DstRM != RM);
        matrix<T, (S ? C : R), (S ? R : C), DstRM, V> r{};

        for(int i = 0; i < R; ++i) {
            for(int j = 0; j < C; ++j) {
                r.set_rm(S ? i : j, S ? j : i, get_rm(i, j));
            }
        }

        return r;
    }

public:
    simd::data_t<T, RM ? C : R, V> _v[RM ? R : C];

    using type = matrix;

    /// @brief Element type.
    using element_type = T;

    /// @brief Creates a matrix from data pointer and size.
    template <typename P>
    [[nodiscard]] static constexpr auto from(
      const P* dt,
      const span_size_t sz) noexcept -> matrix {
        return _from_hlp(dt, sz, _make_iseq < RM ? R : C > {});
    }

    /// @brief Creates a matrix from another matrix type.
    template <typename P, int M, int N, bool W>
    [[nodiscard]] static constexpr auto from(
      const matrix<P, M, N, RM, W>& m) noexcept -> matrix
        requires((C <= M) and (R <= N))
    {
        return _from_hlp(m, _make_iseq < RM ? R : C > {});
    }

    /// @brief Subscript operator.
    [[nodiscard]] auto operator[](const int i) const noexcept
      -> const vector<T, RM ? C : R, V> {
        return {_v[i]};
    }

    /// @brief Indicates if the matrix is row major.
    [[nodiscard]] static constexpr auto is_row_major() noexcept -> bool {
        return RM;
    }

    /// @brief Returns the number of rows in this matrix type.
    /// @see columns
    /// @see dimension
    [[nodiscard]] static constexpr auto rows() noexcept -> int {
        return R;
    }

    /// @brief Returns the number of columns in this matrix type.
    /// @see rows
    /// @see dimension
    [[nodiscard]] static constexpr auto columns() noexcept -> int {
        return C;
    }

    /// @brief Indicates if this matrix is square.
    [[nodiscard]] static constexpr auto is_square() noexcept -> bool {
        return C == R;
    }

    /// @brief Returns the number of columns in this matrix type if it's square.
    /// @see rows
    /// @see columns
    [[nodiscard]] static constexpr auto dimension() noexcept -> int
        requires(is_square())
    {
        return C;
    }

    /// @brief Sets the matrix element at [ri, ci].
    /// @ingroup math
    constexpr auto set_rm(const int ri, const int ci, const T v) noexcept
      -> matrix& {
        assert(ci < C and ri < R);
        if constexpr(is_row_major()) {
            _v[ri][ci] = v;
        } else {
            _v[ci][ri] = v;
        }
        return *this;
    }

    /// @brief Sets the matrix element at [ci, ri].
    /// @ingroup math
    constexpr auto set_cm(const int ci, const int ri, const T v) noexcept
      -> matrix& {
        assert(ci < C and ri < R);
        if constexpr(is_row_major()) {
            _v[ri][ci] = v;
        } else {
            _v[ci][ri] = v;
        }
        return *this;
    }

    /// @brief Gets the matrix element at [ri, ci].
    /// @ingroup math
    constexpr auto get_rm(const int ri, const int ci) const noexcept -> T {
        assert(ci < C and ri < R);
        if constexpr(is_row_major()) {
            return _v[ri][ci];
        } else {
            return _v[ci][ri];
        }
    }

    /// @brief Gets the matrix element at [ci, ri].
    /// @ingroup math
    constexpr auto get_cm(const int ci, const int ri) const noexcept -> T {
        assert(ci < C and ri < R);
        if constexpr(is_row_major()) {
            return _v[ri][ci];
        } else {
            return _v[ci][ri];
        }
    }

    /// @brief Returns this matrix transposed.
    /// @see reordered
    [[nodiscard]] constexpr auto transposed() const noexcept
      -> matrix<T, R, C, RM, V> {
        return _transpose_tpl<RM>();
    }

    /// @brief Returns this matrix reordered (switches row/column major).
    /// @see transposed
    [[nodiscard]] constexpr auto reordered() const noexcept
      -> matrix<T, C, R, not RM, V> {
        return _transpose_tpl<not RM>();
    }

    /// @brief Returns this matrix as row-major.
    /// @see make_column_major
    [[nodiscard]] constexpr auto as_row_major() const noexcept {
        if constexpr(is_row_major()) {
            return *this;
        } else {
            return reordered();
        }
    }

    /// @brief Returns this matrix as column-major.
    /// @see make_row_major
    [[nodiscard]] constexpr auto as_column_major() const noexcept {
        if constexpr(is_row_major()) {
            return reordered();
        } else {
            return *this;
        }
    }

    /// @brief Returns the i-th major vector of this matrix.
    /// @see minor_vector
    [[nodiscard]] constexpr auto major_vector(int i) const noexcept
      -> vector<T, (RM ? C : R), V> {
        assert(i < (RM ? R : C));
        return {_v[i]};
    }

    /// @brief Returns the i-th minor vector of this matrix.
    /// @see major_vector
    [[nodiscard]] constexpr auto minor_vector(int i) const noexcept
      -> vector<T, (RM ? R : C), V> {
        return reordered().major_vector(i);
    }

    /// @brief Returns the i-th row of this matrix.
    /// @see column
    [[nodiscard]] constexpr auto row(int i) const noexcept -> vector<T, C, V> {
        assert(i < R);
        if constexpr(is_row_major()) {
            return major_vector(i);
        } else {
            return minor_vector(i);
        }
    }

    /// @brief Returns the j-th column of this matrix.
    /// @see column
    [[nodiscard]] constexpr auto column(int i) const noexcept
      -> vector<T, R, V> {
        assert(i < C);
        if constexpr(is_row_major()) {
            return minor_vector(i);
        } else {
            return major_vector(i);
        }
    }

    /// @brief Returns the translation vector of this matrix.
    /// @see row
    /// @see column
    [[nodiscard]] constexpr auto translation() const noexcept
      -> std::conditional_t<R == 4, vector<T, 3, V>, vector<T, 2, V>>
        requires(
          ((R == 4) and (C == 3 or C == 4)) or
          ((R == 3) and (C == 2 or C == 3)))
    {
        if constexpr(is_row_major()) {
            if constexpr(R == 4) {
                return vector<T, 3, V>{_v[3]};
            } else {
                return vector<T, 2, V>{_v[2]};
            }
        } else { // column-major
            if constexpr(R == 4) {
                return vector<T, 3, V>{_v[0][3], _v[1][3], _v[2][3]};
            } else {
                return vector<T, 2, V>{_v[0][2], _v[1][2]};
            }
        }
    }

    /// @brief Multiplies this matrix by another matrix.
    template <int K, bool RM2>
    [[nodiscard]] constexpr auto multiply(const matrix<T, K, C, RM2, V>& m)
      const noexcept -> matrix<T, K, R, true, V> {
        if constexpr(RM) {
            if constexpr(not RM2) {
                matrix<T, K, R, RM, V> r{};
                for(int i = 0; i < R; ++i) {
                    for(int j = 0; j < K; ++j) {
                        r.set_rm(i, j, row(i).dot(m.column(j)));
                    }
                }
                return r;
            } else {
                return multiply(m.reordered());
            }
        } else {
            if constexpr(not RM2) {
                return reordered().multiply(m);
            } else {
                return reordered().multiply(m.reordered());
            }
        }
    }

    /// @brief Multiplies a vector by this matrix.
    [[nodiscard]] constexpr auto multiply(
      const vector<T, C, V>& v) const noexcept -> vector<T, R, V> {
        if constexpr(is_row_major()) {
            vector<T, R, V> r{};
            for(int i = 0; i < R; ++i) {
                r._v[i] = row(i).dot(v);
            }
            return r;
        } else {
            return reordered().multiply(v);
        }
    }
};
//------------------------------------------------------------------------------
/// @brief Class testing if a matrix type is row-major.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<matrix<T, C, R, RM, V>> : std::bool_constant<RM> {};

/// @brief Trait indicating if a matrix type is row-major.
/// @ingroup math
export template <typename X>
constexpr const auto is_row_major_v = is_row_major<X>::value;
//------------------------------------------------------------------------------
// reordered matrix trait
export template <typename X>
struct reordered_matrix;
//------------------------------------------------------------------------------
/// @brief Trait returns a reordered (switch row/column-major) type for a matrix type.
/// @ingroup math
export template <typename X>
using reordered_matrix_t = typename reordered_matrix<X>::type;
//------------------------------------------------------------------------------
/// @brief Implementation of reordered_matrix_t.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
struct reordered_matrix<matrix<T, C, R, RM, V>> : matrix<T, R, C, not RM, V> {};
//------------------------------------------------------------------------------
/// @brief Indicates if the specified type @p X is a matrix constructor.
/// @ingroup math
/// @see constructed_matrix_t
export template <typename X>
struct is_matrix_constructor : std::false_type {};

/// @brief Trait indicating if a type X is a matrix constructor.
/// @ingroup math
///
/// Matrix constructor types define a nullary call operator that can construct
/// a matrix instantiation.
export template <typename X>
constexpr const auto is_matrix_constructor_v = is_matrix_constructor<X>::value;
//------------------------------------------------------------------------------
export template <bool RM, typename T, int C, int R, bool V>
struct is_matrix_constructor<matrix<T, C, R, RM, V>> : std::true_type {};
//------------------------------------------------------------------------------
/// @brief Gets the constructed matrix type for a matrix constructor type.
/// @ingroup math
/// @see is_matrix_constructor_v
export template <typename X>
struct constructed_matrix;
//------------------------------------------------------------------------------
/// @brief Trait returning the matrix type constructed by constructor type X.
/// @ingroup math
export template <typename X>
using constructed_matrix_t = typename constructed_matrix<X>::type;
//------------------------------------------------------------------------------
// constructed_matrix trait
export template <typename T, int C, int R, bool RM, bool V>
struct constructed_matrix<matrix<T, C, R, RM, V>> : matrix<T, C, R, RM, V> {};
//------------------------------------------------------------------------------
// construct_matrix (noop)
export template <bool RM, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto construct_matrix(
  const matrix<T, C, R, RM, V>& m) noexcept -> const matrix<T, C, R, RM, V>& {
    return m;
}
//------------------------------------------------------------------------------
// construct_matrix (reorder)
export template <bool RM, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto construct_matrix(
  const matrix<T, C, R, not RM, V>& m) noexcept -> matrix<T, C, R, RM, V> {
    return m.reordered();
}
//------------------------------------------------------------------------------
// are_multiplicable
export template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
struct are_multiplicable<matrix<T, M, K, RM1, V>, matrix<T, K, N, RM2, V>>
  : std::true_type {};
//------------------------------------------------------------------------------
// multiplication_result MxM
export template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
struct multiplication_result<matrix<T, K, M, RM1, V>, matrix<T, N, K, RM2, V>>
  : matrix<T, N, M, RM1, V> {};
//------------------------------------------------------------------------------
/// @brief Matrix multiplication function.
/// @ingroup math
export template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
[[nodiscard]] constexpr auto multiply(
  const matrix<T, K, M, RM1, V>& m1,
  const matrix<T, N, K, RM2, V>& m2) noexcept -> matrix<T, N, M, true, V> {
    return m1.multiply(m2);
}
//------------------------------------------------------------------------------
// are_multiplicable
export template <typename T, int C, int R, bool RM, bool V>
struct are_multiplicable<matrix<T, C, R, RM, V>, vector<T, C, V>>
  : std::true_type {};
//------------------------------------------------------------------------------
// multiplication_result MxV
export template <typename T, int C, int R, bool RM, bool V>
struct multiplication_result<matrix<T, C, R, RM, V>, vector<T, C, V>>
  : vector<T, R, V> {};
//------------------------------------------------------------------------------
/// @brief Matrix-vector multiplication function.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto multiply(
  const matrix<T, C, R, RM, V>& m,
  const vector<T, C, V>& v) noexcept -> vector<T, R, V> {
    return m.multiply(v);
}
//------------------------------------------------------------------------------
/// @brief Multiplication operator for matrix constructors.
/// @ingroup math
///
/// Doing multiplication on matrix constructors is typically more efficient
/// than doing multiplication on the constructed matrices, because it can
/// save row/column reordering operations.
export template <typename MC1, typename MC2>
[[nodiscard]] constexpr auto operator*(const MC1& mc1, const MC2& mc2) noexcept
    requires(
      is_matrix_constructor<MC1>::value and
      is_matrix_constructor<MC2>::value and
      are_multiplicable<constructed_matrix_t<MC1>, constructed_matrix_t<MC2>>::
        value)
{
    return multiply(mc1, mc2);
}
//------------------------------------------------------------------------------
/// @brief Implements gaussian elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_jordan_elimination
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_elimination(
  matrix<T, Ca, R, true, V>& a,
  matrix<T, Cb, R, true, V>& b) noexcept -> bool {
    using std::swap;
    const auto is_zero = [](T x) noexcept {
        return are_equal(x, T(0));
    };

    for(int i = 0; i < R; ++i) {
        auto d = a._v[i][i];
        if(is_zero(d)) {
            for(int k = i + 1; k < R; ++k) {
                if(not is_zero(a._v[k][i])) {
                    swap(a._v[k], a._v[i]);
                    swap(b._v[k], b._v[i]);
                    break;
                }
            }
            d = a._v[i][i];
        }
        if(is_zero(d)) {
            return false;
        }
        const auto inv_d = T(1) / d;

        a._v[i] *= inv_d;
        b._v[i] *= inv_d;

        for(int k = i + 1; k < R; ++k) {
            d = a._v[k][i];
            if(not is_zero(d)) {
                a._v[k] -= a._v[i] * d;
                b._v[k] -= b._v[i] * d;
            }
        }
    }
    return true;
}
//------------------------------------------------------------------------------
/// @brief Implements gaussian elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_jordan_elimination
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_elimination(
  matrix<T, Ca, R, false, V>& a,
  matrix<T, Cb, R, true, V>& b) noexcept -> bool {
    auto ta = a.reordered();
    if(gauss_elimination(ta, b)) {
        a = ta.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gaussian elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_jordan_elimination
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_elimination(
  matrix<T, Ca, R, true, V>& a,
  matrix<T, Cb, R, false, V>& b) noexcept -> bool {
    auto tb = b.reordered();
    if(gauss_elimination(a, tb)) {
        b = tb.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gaussian elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_jordan_elimination
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_elimination(
  matrix<T, Ca, R, false, V>& a,
  matrix<T, Cb, R, false, V>& b) noexcept -> bool {
    auto ta = a.reordered();
    auto tb = b.reordered();
    if(gauss_elimination(ta, tb)) {
        a = ta.reordered();
        b = tb.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gauss-jordan elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_elimination
/// @see inverse_matrix
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_jordan_elimination(
  matrix<T, Ca, R, true, V>& a,
  matrix<T, Cb, R, true, V>& b) noexcept -> bool {

    if(gauss_elimination(a, b)) {

        for(int i = R - 1; i > 0; --i) {
            for(int k = 0; k < i; ++k) {
                const T d = a._v[k][i];
                if(not are_equal(d, T(0))) {
                    a._v[k] -= a._v[i] * d;
                    b._v[k] -= b._v[i] * d;
                }
            }
        }

        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gauss-jordan elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_elimination
/// @see inverse_matrix
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_jordan_elimination(
  matrix<T, Ca, R, false, V>& a,
  matrix<T, Cb, R, true, V>& b) noexcept -> bool {
    auto ta = a.reordered();
    if(gauss_jordan_elimination(ta, b)) {
        a = ta.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gauss-jordan elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_elimination
/// @see inverse_matrix
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_jordan_elimination(
  matrix<T, Ca, R, true, V>& a,
  matrix<T, Cb, R, false, V>& b) noexcept -> bool {
    auto tb = b.reordered();
    if(gauss_jordan_elimination(a, tb)) {
        b = tb.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Implements gauss-jordan elimination on matrices @p a and @p b
/// @ingroup math
/// @see gauss_elimination
/// @see inverse_matrix
export template <typename T, int Ca, int Cb, int R, bool V>
[[nodiscard]] constexpr auto gauss_jordan_elimination(
  matrix<T, Ca, R, false, V>& a,
  matrix<T, Cb, R, false, V>& b) noexcept -> bool {
    auto ta = a.reordered();
    auto tb = b.reordered();
    if(gauss_jordan_elimination(ta, tb)) {
        a = ta.reordered();
        b = tb.reordered();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace math
//------------------------------------------------------------------------------
export template <typename T, int C, int R, bool RM, bool V>
struct is_known_matrix_type<math::matrix<T, C, R, RM, V>>
  : std::is_scalar<T> {};

export template <typename T, int C, int R, bool RM, bool V>
struct canonical_compound_type<math::matrix<T, C, R, RM, V>>
  : std::type_identity<std::remove_cv_t<T[C][R]>> {};

export template <typename T, int C, int R, bool RM, bool V>
struct compound_view_maker<math::matrix<T, C, R, RM, V>> {
    [[nodiscard]] auto operator()(
      const math::matrix<T, C, R, RM, V>& m) const noexcept {
        return simd::view < T, RM ? C : R, V > ::apply(m._v);
    }
};

export template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<math::matrix<T, C, R, RM, V>> : std::bool_constant<RM> {};
//------------------------------------------------------------------------------
} // namespace eagine

