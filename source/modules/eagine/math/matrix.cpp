/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:matrix;

import std;
import eagine.core.concepts;
import eagine.core.types;
import eagine.core.vectorization;

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
struct matrix {
    using type = matrix;

    /// @brief Element type.
    using element_type = T;

    template <int... U>
    using _iseq = std::integer_sequence<int, U...>;

    template <int N>
    using _make_iseq = std::make_integer_sequence<int, N>;

    vect::data_t<T, RM ? C : R, V> _v[RM ? R : C];

    template <typename P, int... I>
    static auto _from_hlp(const P* dt, span_size_t sz, _iseq<I...>) noexcept
      -> matrix {
        return matrix{
          {vect::from_array < T,
           RM ? C : R,
           V > ::apply(dt + I * (RM ? C : R), sz - I * (RM ? C : R))...}};
    }

    /// @brief Creates a matrix from data pointer and size.
    template <typename P>
    [[nodiscard]] static auto from(const P* dt, const span_size_t sz) noexcept
      -> matrix {
        return _from_hlp(dt, sz, _make_iseq < RM ? R : C > ());
    }

    template <typename P, int M, int N, bool W, int... I>
    static auto _from_hlp(const matrix<P, M, N, RM, W>& m, _iseq<I...>) noexcept
      -> matrix {
        return matrix{
          {vect::cast<P, (RM ? M : N), W, T, (RM ? C : R), V>::apply(
            m._v[I], T(0))...}};
    }

    /// @brief Creates a matrix from another matrix type.
    template <typename P, int M, int N, bool W>
    [[nodiscard]] static auto from(const matrix<P, M, N, RM, W>& m) noexcept
      -> matrix
        requires((C <= M) and (R <= N))
    {
        return _from_hlp(m, _make_iseq < RM ? R : C > ());
    }

    /// @brief Subscript operator.
    [[nodiscard]] auto operator[](const int i) const noexcept
      -> const vector<T, RM ? C : R, V> {
        return {_v[i]};
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
/// @brief Returns then number of matrix rows.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto rows(const matrix<T, C, R, RM, V>&) noexcept {
    return R;
}
//------------------------------------------------------------------------------
/// @brief Returns then number of matrix columns.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto columns(const matrix<T, C, R, RM, V>&) noexcept {
    return C;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a matrix is row-major.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto row_major(const matrix<T, C, R, RM, V>&) noexcept {
    return RM;
}
//------------------------------------------------------------------------------
/// @brief Returns the dimension of a square matrix.
/// @ingroup math
export template <typename T, int N, bool RM, bool V>
[[nodiscard]] constexpr auto dimension(const matrix<T, N, N, RM, V>&) noexcept {
    return N;
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [CI, RI]. Column-major implementation.
/// @ingroup math
export template <int CI, int RI, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_cm(const matrix<T, C, R, false, V>& m) noexcept
  -> T
    requires(CI < C and RI < R)
{
    return m._v[CI][RI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [CI, RI]. Row-major implementation.
/// @ingroup math
export template <int CI, int RI, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_cm(const matrix<T, C, R, true, V>& m) noexcept
  -> T
    requires(CI < C and RI < R)
{
    return m._v[RI][CI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ci, ri]. Column-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_cm(
  const matrix<T, C, R, false, V>& m,
  const int ci,
  const int ri) noexcept -> T {
    assert(ci < C and ri < R);
    return m._v[ci][ri];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ci, ri]. Row-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_cm(
  const matrix<T, C, R, true, V>& m,
  const int ci,
  const int ri) noexcept -> T {
    assert(ci < C and ri < R);
    return m._v[ri][ci];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [RI, CI]. Column-major implementation.
/// @ingroup math
export template <int RI, int CI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
[[nodiscard]] constexpr auto get_rm(const matrix<T, C, R, false, V>& m) noexcept
  -> T {
    return m._v[CI][RI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [RI, CI]. Row-major implementation.
/// @ingroup math
export template <int RI, int CI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
[[nodiscard]] constexpr auto get_rm(const matrix<T, C, R, true, V>& m) noexcept
  -> T {
    return m._v[RI][CI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ri, ci]. Column-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_rm(
  const matrix<T, C, R, false, V>& m,
  const int ri,
  const int ci) noexcept -> T {
    assert(ci < C and ri < R);
    return m._v[ci][ri];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ri, ci]. Row-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto get_rm(
  const matrix<T, C, R, true, V>& m,
  const int ri,
  const int ci) noexcept -> T {
    assert(ci < C and ri < R);
    return m._v[ri][ci];
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [CI, RI]. Column-major implementation.
/// @ingroup math
export template <int CI, int RI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
constexpr void set_cm(matrix<T, C, R, false, V>& m, const T v) noexcept {
    m._v[CI][RI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [CI, RI]. Row-major implementation.
/// @ingroup math
export template <int CI, int RI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
constexpr void set_cm(matrix<T, C, R, true, V>& m, const T v) noexcept {
    m._v[RI][CI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ci, ri]. Column-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
constexpr void set_cm(
  matrix<T, C, R, false, V>& m,
  const int ci,
  const int ri,
  const T v) noexcept {
    assert(ci < C and ri < R);
    m._v[ci][ri] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ci, ri]. Row-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
constexpr void set_cm(
  matrix<T, C, R, true, V>& m,
  const int ci,
  const int ri,
  const T v) noexcept {
    assert(ci < C and ri < R);
    m._v[ri][ci] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [RI, CI]. Column-major implementation.
/// @ingroup math
export template <int RI, int CI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
constexpr void set_rm(matrix<T, C, R, false, V>& m, const T v) noexcept {
    m._v[CI][RI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [RI, CI]. Row-major implementation.
/// @ingroup math
export template <int RI, int CI, typename T, int C, int R, bool V>
    requires(CI < C and RI < R)
constexpr void set_rm(matrix<T, C, R, true, V>& m, const T v) noexcept {
    m._v[RI][CI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ri, ci]. Column-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
constexpr void set_rm(
  matrix<T, C, R, false, V>& m,
  const int ri,
  const int ci,
  const T v) noexcept {
    assert(ci < C and ri < R);
    m._v[ci][ri] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ri, ci]. Row-major implementation.
/// @ingroup math
export template <typename T, int C, int R, bool V>
constexpr void set_rm(
  matrix<T, C, R, true, V>& m,
  const int ri,
  const int ci,
  const T v) noexcept {
    assert(ci < C and ri < R);
    m._v[ri][ci] = v;
}
//------------------------------------------------------------------------------
// transpose_tpl helper 4x4 matrix
export template <bool DstRM, typename T, bool V>
constexpr auto transpose_tpl_hlp(
  const vect::data_t<T, 4, V>& q0,
  const vect::data_t<T, 4, V>& q1,
  const vect::data_t<T, 4, V>& q2,
  const vect::data_t<T, 4, V>& q3) noexcept {
    using vect::shuffle2;
    using vect::shuffle_mask;
    return matrix<T, 4, 4, DstRM, V>{
      {shuffle2<T, 4, V>::apply(q0, q2, shuffle_mask<0, 2, 4, 6>{}),
       shuffle2<T, 4, V>::apply(q0, q2, shuffle_mask<1, 3, 5, 7>{}),
       shuffle2<T, 4, V>::apply(q1, q3, shuffle_mask<0, 2, 4, 6>{}),
       shuffle2<T, 4, V>::apply(q1, q3, shuffle_mask<1, 3, 5, 7>{})}};
}
//------------------------------------------------------------------------------
// transpose_tpl 4x4 matrix
export template <bool DstRM, bool SrcRM, typename T, bool V>
constexpr auto transpose_tpl(const matrix<T, 4, 4, SrcRM, V>& m) noexcept
  -> matrix<T, 4, 4, DstRM, V> {
    using vect::shuffle2;
    using vect::shuffle_mask;
    return transpose_tpl_hlp<DstRM, T, V>(
      shuffle2<T, 4, V>::apply(m._v[0], m._v[1], shuffle_mask<0, 1, 4, 5>{}),
      shuffle2<T, 4, V>::apply(m._v[0], m._v[1], shuffle_mask<2, 3, 6, 7>{}),
      shuffle2<T, 4, V>::apply(m._v[2], m._v[3], shuffle_mask<0, 1, 4, 5>{}),
      shuffle2<T, 4, V>::apply(m._v[2], m._v[3], shuffle_mask<2, 3, 6, 7>{}));
}
//------------------------------------------------------------------------------
// transpose_tpl
export template <bool DstRM, bool SrcRM, typename T, int C, int R, bool V>
constexpr auto transpose_tpl(const matrix<T, C, R, SrcRM, V>& m) noexcept
  -> matrix<T, (DstRM == SrcRM ? R : C), (DstRM == SrcRM ? C : R), DstRM, V> {
    const bool S = (DstRM != SrcRM);
    matrix<T, (S ? C : R), (S ? R : C), DstRM, V> r;

    for(int i = 0; i < R; ++i) {
        for(int j = 0; j < C; ++j) {
            set_rm(r, S ? i : j, S ? j : i, get_rm(m, i, j));
        }
    }

    return r;
}
//------------------------------------------------------------------------------
/// @brief Transposes a matrix.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto transpose(const matrix<T, C, R, RM, V>& m) noexcept
  -> matrix<T, R, C, RM, V> {
    return transpose_tpl<RM, RM, T>(m);
}
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
/// @brief Returns a matrix reordered (switches row/column major).
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto reorder(const matrix<T, C, R, RM, V>& m) noexcept
  -> matrix<T, C, R, not RM, V> {
    return transpose_tpl<not RM, RM, T>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as row-major.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto make_row_major(matrix<T, C, R, true, V> m) noexcept
  -> matrix<T, C, R, true, V> {
    return m;
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as row-major.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto make_row_major(
  const matrix<T, C, R, false, V>& m) noexcept -> matrix<T, C, R, true, V> {
    return reorder(m);
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as column-major.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto make_column_major(
  matrix<T, C, R, false, V> m) noexcept -> matrix<T, C, R, false, V> {
    return m;
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as column-major.
/// @ingroup math
export template <typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto make_column_major(
  const matrix<T, C, R, true, V>& m) noexcept -> matrix<T, C, R, false, V> {
    return reorder(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th major vector of a matrix.
/// @ingroup math
export template <int I, typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto major_vector(
  const matrix<T, C, R, RM, V>& m) noexcept -> vector<T, (RM ? C : R), V>
    requires(I < (RM ? R : C))
{
    return {m._v[I]};
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th minor vector of a matrix.
/// @ingroup math
export template <int I, typename T, int C, int R, bool RM, bool V>
[[nodiscard]] constexpr auto minor_vector(
  const matrix<T, C, R, RM, V>& m) noexcept -> vector<T, (RM ? R : C), V>
    requires(I < (RM ? C : R))
{
    return major_vector<I>(reorder(m));
}
//------------------------------------------------------------------------------
// minor_vector mat4x4
export template <int I, typename T, bool RM, bool V>
[[nodiscard]] constexpr auto minor_vector(
  const matrix<T, 4, 4, RM, V>& m) noexcept -> vector<T, 4, V>
    requires(I < 4)
{
    return {vect::shuffle2<T, 4, V>::apply(
      vect::shuffle2<T, 4, V>::apply(
        m._v[0], m._v[1], vect::shuffle_mask<0 + I, 4 + I, -1, -1>{}),
      vect::shuffle2<T, 4, V>::apply(
        m._v[2], m._v[3], vect::shuffle_mask<0 + I, 4 + I, -1, -1>{}),
      vect::shuffle_mask<0, 1, 4, 5>{})};
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th row vector of a matrix. Row-major implementation.
/// @ingroup math
export template <int I, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto row(const matrix<T, C, R, true, V>& m) noexcept
  -> vector<T, C, V> {
    static_assert(I < R);
    return major_vector<I>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th row vector of a matrix. Column-major implementation.
/// @ingroup math
export template <int I, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto row(const matrix<T, C, R, false, V>& m) noexcept
  -> vector<T, C, V> {
    static_assert(I < R);
    return minor_vector<I>(m);
}
//------------------------------------------------------------------------------
// _row_hlp
template <typename T, int C, int R, bool RM, bool V>
constexpr auto _row_hlp(
  const matrix<T, C, R, RM, V>& m,
  const int_constant<0U>,
  [[maybe_unused]] const int i) noexcept -> vector<T, C, V> {
    assert(i == 0U);
    return row<0U>(m);
}
//------------------------------------------------------------------------------
// _row_hlp
template <typename T, int R, int C, bool RM, bool V, int I>
constexpr auto _row_hlp(
  const matrix<T, C, R, RM, V>& m,
  const int_constant<I>,
  const int i) noexcept -> vector<T, C, V> {
    if(I == i) {
        return row<I>(m);
    }
    return _row_hlp(m, int_constant<I - 1>(), i);
}
//------------------------------------------------------------------------------
/// @brief Returns the i-th row vector of a matrix.
/// @ingroup math
export template <typename T, int R, int C, bool RM, bool V>
[[nodiscard]] constexpr auto row(
  const matrix<T, C, R, RM, V>& m,
  const int i) noexcept -> vector<T, C, V> {
    return _row_hlp(m, int_constant<R - 1>(), i);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th column vector of a matrix. Column-major implementation.
/// @ingroup math
export template <int I, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto column(const matrix<T, C, R, false, V>& m) noexcept
  -> vector<T, R, V> {
    return major_vector<I>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th column vector of a matrix. Row-major implementation.
/// @ingroup math
export template <int I, typename T, int C, int R, bool V>
[[nodiscard]] constexpr auto column(const matrix<T, C, R, true, V>& m) noexcept
  -> vector<T, R, V> {
    return minor_vector<I>(m);
}
//------------------------------------------------------------------------------
// _col_hlp
export template <typename T, int C, int R, bool RM, bool V>
constexpr auto _col_hlp(
  const matrix<T, C, R, RM, V>& m,
  int_constant<0U>,
  [[maybe_unused]] const int i) noexcept -> vector<T, R, V> {
    assert(i == 0);
    return column<0>(m);
}
//------------------------------------------------------------------------------
// _col_hlp
export template <typename T, int C, int R, bool RM, bool V, int I>
constexpr auto _col_hlp(
  const matrix<T, C, R, RM, V>& m,
  int_constant<I>,
  const int i) noexcept -> vector<T, R, V> {
    if(I == i) {
        return column<I>(m);
    }
    return _col_hlp(m, int_constant<I - 1>(), i);
}
//------------------------------------------------------------------------------
/// @brief Returns the i-th column vector of a matrix.
/// @ingroup math
export template <typename T, int R, int C, bool RM, bool V>
[[nodiscard]] constexpr auto column(
  const matrix<T, C, R, RM, V>& m,
  const int i) noexcept -> vector<T, R, V> {
    return _col_hlp(m, int_constant<C - 1>(), i);
}
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
    return reorder(m);
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
  const matrix<T, N, K, RM2, V>& m2) noexcept -> matrix<T, N, M, RM1, V> {
    matrix<T, N, M, RM1, V> m3{};

    for(int i = 0; i < M; ++i) {
        for(int j = 0; j < N; ++j) {
            T s = T(0);

            for(int k = 0; k < K; ++k) {
                s += get_rm(m1, i, k) * get_rm(m2, k, j);
            }

            set_rm(m3, i, j, s);
        }
    }
    return m3;
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
    vector<T, R, V> r{};

    for(int i = 0; i < R; ++i) {
        T s = T(0);
        for(int j = 0; j < C; ++j) {
            s += get_rm(m, i, j) * v._v[j];
        }
        r._v[i] = s;
    }
    return r;
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
    auto ta = reorder(a);
    if(gauss_elimination(ta, b)) {
        a = reorder(ta);
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
    auto tb = reorder(b);
    if(gauss_elimination(a, tb)) {
        b = reorder(tb);
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
    auto ta = reorder(a);
    auto tb = reorder(b);
    if(gauss_elimination(ta, tb)) {
        a = reorder(ta);
        b = reorder(tb);
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
    auto ta = reorder(a);
    if(gauss_jordan_elimination(ta, b)) {
        a = reorder(ta);
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
    auto tb = reorder(b);
    if(gauss_jordan_elimination(a, tb)) {
        b = reorder(tb);
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
    auto ta = reorder(a);
    auto tb = reorder(b);
    if(gauss_jordan_elimination(ta, tb)) {
        a = reorder(ta);
        b = reorder(tb);
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
  : std::type_identity<std::remove_cv_t<T[C][R]>>{};

export template <typename T, int C, int R, bool RM, bool V>
struct compound_view_maker<math::matrix<T, C, R, RM, V>> {
    [[nodiscard]] auto operator()(
      const math::matrix<T, C, R, RM, V>& m) const noexcept {
        return vect::view < T, RM ? C : R, V > ::apply(m._v);
    }
};

export template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<math::matrix<T, C, R, RM, V>> : std::bool_constant<RM> {};
//------------------------------------------------------------------------------
} // namespace eagine

