/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MATH_MATRIX_HPP
#define EAGINE_MATH_MATRIX_HPP

#include "../assert.hpp"
#include "../maybe_unused.hpp"
#include "vector.hpp"
#include <utility>

namespace eagine {
namespace math {
//------------------------------------------------------------------------------
/// @brief Basic RxC matrix implementation template.
/// @ingroup math
/// @see tmat
/// @note This is a base class, typically tmat should be used in client code.
template <typename T, int C, int R, bool RM, bool V>
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
    static auto from(const P* dt, const span_size_t sz) noexcept -> matrix {
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
    static auto from(const matrix<P, M, N, RM, W>& m) noexcept
      -> std::enable_if_t<(C <= M) && (R <= N), matrix> {
        return _from_hlp(m, _make_iseq < RM ? R : C > ());
    }

    /// @brief Subscript operator.
    auto operator[](const int i) const noexcept
      -> const vector<T, RM ? C : R, V> {
        return {_v[i]};
    }
};
//------------------------------------------------------------------------------
/// @brief Class testing if a matrix type is row-major.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<matrix<T, C, R, RM, V>> : bool_constant<RM> {};

/// @brief Trait indicating if a matrix type is row-major.
/// @ingroup math
template <typename X>
const auto is_row_major_v = is_row_major<X>::value;
//------------------------------------------------------------------------------
/// @brief Returns then number of matrix rows.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
static constexpr auto rows(const matrix<T, C, R, RM, V>&) noexcept {
    return R;
}
//------------------------------------------------------------------------------
/// @brief Returns then number of matrix columns.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
static constexpr auto columns(const matrix<T, C, R, RM, V>&) noexcept {
    return C;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a matrix is row-major.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
static constexpr auto row_major(const matrix<T, C, R, RM, V>&) noexcept {
    return RM;
}
//------------------------------------------------------------------------------
/// @brief Returns the dimension of a square matrix.
/// @ingroup math
template <typename T, int N, bool RM, bool V>
static constexpr auto dimension(const matrix<T, N, N, RM, V>&) noexcept {
    return N;
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [CI, RI]. Column-major implementation.
/// @ingroup math
template <int CI, int RI, typename T, int C, int R, bool V>
static constexpr auto get_cm(const matrix<T, C, R, false, V>& m) noexcept
  -> std::enable_if_t<(CI < C && RI < R), T> {
    return m._v[CI][RI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [CI, RI]. Row-major implementation.
/// @ingroup math
template <int CI, int RI, typename T, int C, int R, bool V>
static constexpr auto get_cm(const matrix<T, C, R, true, V>& m) noexcept
  -> std::enable_if_t<(CI < C && RI < R), T> {
    return m._v[RI][CI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ci, ri]. Column-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto get_cm(
  const matrix<T, C, R, false, V>& m,
  const int ci,
  const int ri) noexcept -> T {
    EAGINE_ASSERT(ci < C && ri < R);
    return m._v[ci][ri];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ci, ri]. Row-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto get_cm(
  const matrix<T, C, R, true, V>& m,
  const int ci,
  const int ri) noexcept -> T {
    EAGINE_ASSERT(ci < C && ri < R);
    return m._v[ri][ci];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [RI, CI]. Column-major implementation.
/// @ingroup math
template <int RI, int CI, typename T, int C, int R, bool V>
static constexpr auto get_rm(const matrix<T, C, R, false, V>& m) noexcept
  -> std::enable_if_t<(CI < C && RI < R), T> {
    return m._v[CI][RI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [RI, CI]. Row-major implementation.
/// @ingroup math
template <int RI, int CI, typename T, int C, int R, bool V>
static constexpr auto get_rm(const matrix<T, C, R, true, V>& m) noexcept
  -> std::enable_if_t<(CI < C && RI < R), T> {
    return m._v[RI][CI];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ri, ci]. Column-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto get_rm(
  const matrix<T, C, R, false, V>& m,
  const int ri,
  const int ci) noexcept -> T {
    EAGINE_ASSERT(ci < C && ri < R);
    return m._v[ci][ri];
}
//------------------------------------------------------------------------------
/// @brief Returns the matrix element at [ri, ci]. Row-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto get_rm(
  const matrix<T, C, R, true, V>& m,
  const int ri,
  const int ci) noexcept -> T {
    EAGINE_ASSERT(ci < C && ri < R);
    return m._v[ri][ci];
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [CI, RI]. Column-major implementation.
/// @ingroup math
template <int CI, int RI, typename T, int C, int R, bool V>
static inline auto set_cm(matrix<T, C, R, false, V>& m, const T v) noexcept
  -> std::enable_if_t<(CI < C && RI < R)> {
    m._v[CI][RI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [CI, RI]. Row-major implementation.
/// @ingroup math
template <int CI, int RI, typename T, int C, int R, bool V>
static inline auto set_cm(matrix<T, C, R, true, V>& m, const T v) noexcept
  -> std::enable_if_t<(CI < C && RI < R)> {
    m._v[RI][CI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ci, ri]. Column-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline void set_cm(
  matrix<T, C, R, false, V>& m,
  const int ci,
  const int ri,
  const T v) noexcept {
    EAGINE_ASSERT(ci < C && ri < R);
    m._v[ci][ri] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ci, ri]. Row-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline void set_cm(
  matrix<T, C, R, true, V>& m,
  const int ci,
  const int ri,
  const T v) noexcept {
    EAGINE_ASSERT(ci < C && ri < R);
    m._v[ri][ci] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [RI, CI]. Column-major implementation.
/// @ingroup math
template <int RI, int CI, typename T, int C, int R, bool V>
static inline auto set_rm(matrix<T, C, R, false, V>& m, const T v) noexcept
  -> std::enable_if_t<(CI < C && RI < R)> {
    m._v[CI][RI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [RI, CI]. Row-major implementation.
/// @ingroup math
template <int RI, int CI, typename T, int C, int R, bool V>
static inline auto set_rm(matrix<T, C, R, true, V>& m, const T v) noexcept
  -> std::enable_if_t<(CI < C && RI < R)> {
    m._v[RI][CI] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ri, ci]. Column-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline void set_rm(
  matrix<T, C, R, false, V>& m,
  const int ri,
  const int ci,
  const T v) noexcept {
    EAGINE_ASSERT(ci < C && ri < R);
    m._v[ci][ri] = v;
}
//------------------------------------------------------------------------------
/// @brief Sets the matrix element at [ri, ci]. Row-major implementation.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline void set_rm(
  matrix<T, C, R, true, V>& m,
  const int ri,
  const int ci,
  const T v) noexcept {
    EAGINE_ASSERT(ci < C && ri < R);
    m._v[ri][ci] = v;
}
//------------------------------------------------------------------------------
// transpose_tpl helper 4x4 matrix
template <bool DstRM, typename T, bool V>
static inline auto transpose_tpl_hlp(
  const vect::data_t<T, 4, V>& q0,
  const vect::data_t<T, 4, V>& q1,
  const vect::data_t<T, 4, V>& q2,
  const vect::data_t<T, 4, V>& q3) noexcept {
    return matrix<T, 4, 4, DstRM, V>{
      {vect::shuffle2<T, 4, V>::template apply<0, 2, 4, 6>(q0, q2),
       vect::shuffle2<T, 4, V>::template apply<1, 3, 5, 7>(q0, q2),
       vect::shuffle2<T, 4, V>::template apply<0, 2, 4, 6>(q1, q3),
       vect::shuffle2<T, 4, V>::template apply<1, 3, 5, 7>(q1, q3)}};
}
//------------------------------------------------------------------------------
// transpose_tpl 4x4 matrix
template <bool DstRM, bool SrcRM, typename T, bool V>
static inline auto transpose_tpl(const matrix<T, 4, 4, SrcRM, V>& m) noexcept
  -> matrix<T, 4, 4, DstRM, V> {
    return transpose_tpl_hlp<DstRM, T, V>(
      vect::shuffle2<T, 4, V>::template apply<0, 1, 4, 5>(m._v[0], m._v[1]),
      vect::shuffle2<T, 4, V>::template apply<2, 3, 6, 7>(m._v[0], m._v[1]),
      vect::shuffle2<T, 4, V>::template apply<0, 1, 4, 5>(m._v[2], m._v[3]),
      vect::shuffle2<T, 4, V>::template apply<2, 3, 6, 7>(m._v[2], m._v[3]));
}
//------------------------------------------------------------------------------
// transpose_tpl
template <bool DstRM, bool SrcRM, typename T, int C, int R, bool V>
static inline auto transpose_tpl(const matrix<T, C, R, SrcRM, V>& m) noexcept
  -> matrix<T, (DstRM == SrcRM ? R : C), (DstRM == SrcRM ? C : R), DstRM, V> {
    static const bool S = (DstRM != SrcRM);
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
template <typename T, int C, int R, bool RM, bool V>
static inline auto transpose(const matrix<T, C, R, RM, V>& m) noexcept
  -> matrix<T, R, C, RM, V> {
    return transpose_tpl<RM, RM, T>(m);
}
//------------------------------------------------------------------------------
// reordered matrix trait
template <typename X>
struct reordered_matrix;
//------------------------------------------------------------------------------
/// @brief Trait returns a reordered (switch row/column-major) type for a matrix type.
/// @ingroup math
template <typename X>
using reordered_matrix_t = typename reordered_matrix<X>::type;
//------------------------------------------------------------------------------
/// @brief Implementation of reordered_matrix_t.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
struct reordered_matrix<matrix<T, C, R, RM, V>> : matrix<T, R, C, !RM, V> {};
//------------------------------------------------------------------------------
/// @brief Returns a matrix reordered (switches row/column major).
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
static inline auto reorder(const matrix<T, C, R, RM, V>& m) noexcept
  -> matrix<T, C, R, !RM, V> {
    return transpose_tpl<!RM, RM, T>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as row-major.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto make_row_major(matrix<T, C, R, true, V> m) noexcept
  -> matrix<T, C, R, true, V> {
    return m;
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as row-major.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline auto make_row_major(const matrix<T, C, R, false, V>& m) noexcept
  -> matrix<T, C, R, true, V> {
    return reorder(m);
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as column-major.
/// @ingroup math
template <typename T, int C, int R, bool V>
static constexpr auto make_column_major(matrix<T, C, R, false, V> m) noexcept
  -> matrix<T, C, R, false, V> {
    return m;
}
//------------------------------------------------------------------------------
/// @brief Returns a matrix ordered as column-major.
/// @ingroup math
template <typename T, int C, int R, bool V>
static inline auto make_column_major(const matrix<T, C, R, true, V>& m) noexcept
  -> matrix<T, C, R, false, V> {
    return reorder(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th major vector of a matrix.
/// @ingroup math
template <int I, typename T, int C, int R, bool RM, bool V>
static constexpr auto major_vector(const matrix<T, C, R, RM, V>& m) noexcept
  -> std::enable_if_t<(I < (RM ? R : C)), vector<T, (RM ? C : R), V>> {
    return {m._v[I]};
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th minor vector of a matrix.
/// @ingroup math
template <int I, typename T, int C, int R, bool RM, bool V>
static inline auto minor_vector(const matrix<T, C, R, RM, V>& m) noexcept
  -> std::enable_if_t<(I < (RM ? C : R)), vector<T, (RM ? R : C), V>> {
    return major_vector<I>(reorder(m));
}
//------------------------------------------------------------------------------
// minor_vector mat4x4
template <int I, typename T, bool RM, bool V>
static inline auto minor_vector(const matrix<T, 4, 4, RM, V>& m) noexcept
  -> std::enable_if_t<(I < 4), vector<T, 4, V>> {
    return {vect::shuffle2<T, 4, V>::template apply<0, 1, 4, 5>(
      vect::shuffle2<T, 4, V>::template apply<0 + I, 4 + I, -1, -1>(
        m._v[0], m._v[1]),
      vect::shuffle2<T, 4, V>::template apply<0 + I, 4 + I, -1, -1>(
        m._v[2], m._v[3]))};
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th row vector of a matrix. Row-major implementation.
/// @ingroup math
template <int I, typename T, int C, int R, bool V>
static constexpr auto row(const matrix<T, C, R, true, V>& m) noexcept
  -> vector<T, C, V> {
    static_assert(I < R);
    return major_vector<I>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th row vector of a matrix. Column-major implementation.
/// @ingroup math
template <int I, typename T, int C, int R, bool V>
static inline auto row(const matrix<T, C, R, false, V>& m) noexcept
  -> vector<T, C, V> {
    static_assert(I < R);
    return minor_vector<I>(m);
}
//------------------------------------------------------------------------------
// _row_hlp
template <typename T, int C, int R, bool RM, bool V>
static inline auto _row_hlp(
  const matrix<T, C, R, RM, V>& m,
  const int_constant<0U>,
  const int i) noexcept -> vector<T, C, V> {
    EAGINE_ASSERT(i == 0U);
    EAGINE_MAYBE_UNUSED(i);
    return row<0U>(m);
}
//------------------------------------------------------------------------------
// _row_hlp
template <typename T, int R, int C, bool RM, bool V, int I>
static inline auto _row_hlp(
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
template <typename T, int R, int C, bool RM, bool V>
static inline auto row(const matrix<T, C, R, RM, V>& m, const int i) noexcept
  -> vector<T, C, V> {
    return _row_hlp(m, int_constant<R - 1>(), i);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th column vector of a matrix. Column-major implementation.
/// @ingroup math
template <int I, typename T, int C, int R, bool V>
static constexpr auto column(const matrix<T, C, R, false, V>& m) noexcept
  -> vector<T, R, V> {
    return major_vector<I>(m);
}
//------------------------------------------------------------------------------
/// @brief Returns the I-th column vector of a matrix. Row-major implementation.
/// @ingroup math
template <int I, typename T, int C, int R, bool V>
static inline auto column(const matrix<T, C, R, true, V>& m) noexcept
  -> vector<T, R, V> {
    return minor_vector<I>(m);
}
//------------------------------------------------------------------------------
// _col_hlp
template <typename T, int C, int R, bool RM, bool V>
static inline auto _col_hlp(
  const matrix<T, C, R, RM, V>& m,
  int_constant<0U>,
  const int i) noexcept -> vector<T, R, V> {
    EAGINE_ASSERT(i == 0);
    EAGINE_MAYBE_UNUSED(i);
    return column<0>(m);
}
//------------------------------------------------------------------------------
// _col_hlp
template <typename T, int C, int R, bool RM, bool V, int I>
static inline auto _col_hlp(
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
template <typename T, int R, int C, bool RM, bool V>
static inline auto column(const matrix<T, C, R, RM, V>& m, const int i) noexcept
  -> vector<T, R, V> {
    return _col_hlp(m, int_constant<C - 1>(), i);
}
//------------------------------------------------------------------------------
/// @brief Indicates if the specified type @p X is a matrix constructor.
/// @ingroup math
/// @see constructed_matrix_t
template <typename X>
struct is_matrix_constructor : std::false_type {};

/// @brief Trait indicating if a type X is a matrix constructor.
/// @ingroup math
///
/// Matrix constructor types define a nullary call operator that can construct
/// a matrix instantiation.
template <typename X>
constexpr auto is_matrix_constructor_v = is_matrix_constructor<X>::value;
//------------------------------------------------------------------------------
template <bool RM, typename T, int C, int R, bool V>
struct is_matrix_constructor<matrix<T, C, R, RM, V>> : std::true_type {};
//------------------------------------------------------------------------------
/// @brief Gets the constructed matrix type for a matrix constructor type.
/// @ingroup math
/// @see is_matrix_constructor_v
template <typename X>
struct constructed_matrix;
//------------------------------------------------------------------------------
/// @brief Trait returning the matrix type constructed by constructor type X.
/// @ingroup math
template <typename X>
using constructed_matrix_t = typename constructed_matrix<X>::type;
//------------------------------------------------------------------------------
// constructed_matrix trait
template <typename T, int C, int R, bool RM, bool V>
struct constructed_matrix<matrix<T, C, R, RM, V>> : matrix<T, C, R, RM, V> {};
//------------------------------------------------------------------------------
// construct_matrix (noop)
template <bool RM, typename T, int C, int R, bool V>
static constexpr auto construct_matrix(const matrix<T, C, R, RM, V>& m) noexcept
  -> const matrix<T, C, R, RM, V>& {
    return m;
}
//------------------------------------------------------------------------------
// construct_matrix (reorder)
template <bool RM, typename T, int C, int R, bool V>
static constexpr auto construct_matrix(const matrix<T, C, R, !RM, V>& m) noexcept
  -> matrix<T, C, R, RM, V> {
    return reorder(m);
}
//------------------------------------------------------------------------------
// are_multiplicable
template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
struct are_multiplicable<matrix<T, M, K, RM1, V>, matrix<T, K, N, RM2, V>>
  : std::true_type {};
//------------------------------------------------------------------------------
// multiplication_result MxM
template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
struct multiplication_result<matrix<T, K, M, RM1, V>, matrix<T, N, K, RM2, V>>
  : matrix<T, N, M, RM1, V> {};
//------------------------------------------------------------------------------
/// @brief Matrix multiplication function.
/// @ingroup math
template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
static inline auto multiply(
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
template <typename T, int C, int R, bool RM, bool V>
struct are_multiplicable<matrix<T, C, R, RM, V>, vector<T, C, V>>
  : std::true_type {};
//------------------------------------------------------------------------------
// multiplication_result MxV
template <typename T, int C, int R, bool RM, bool V>
struct multiplication_result<matrix<T, C, R, RM, V>, vector<T, C, V>>
  : vector<T, R, V> {};
//------------------------------------------------------------------------------
/// @brief Matrix-vector multiplication function.
/// @ingroup math
template <typename T, int C, int R, bool RM, bool V>
static inline auto multiply(
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
template <
  typename MC1,
  typename MC2,
  typename = std::enable_if_t<
    is_matrix_constructor<MC1>::value && is_matrix_constructor<MC2>::value &&
    are_multiplicable<constructed_matrix_t<MC1>, constructed_matrix_t<MC2>>::value>>
static inline auto operator*(const MC1& mc1, const MC2& mc2) noexcept {
    return multiply(mc1, mc2);
}
//------------------------------------------------------------------------------
} // namespace math
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V>
struct is_known_matrix_type<math::matrix<T, C, R, RM, V>>
  : std::is_scalar<T> {};
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V>
struct canonical_compound_type<math::matrix<T, C, R, RM, V>>
  : type_identity<std::remove_cv_t<T[C][R]>> {};
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V>
struct compound_view_maker<math::matrix<T, C, R, RM, V>> {
    auto operator()(const math::matrix<T, C, R, RM, V>& m) const noexcept {
        return vect::view < T, RM ? C : R, V > ::apply(m._v);
    }
};
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<math::matrix<T, C, R, RM, V>> : bool_constant<RM> {};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_MATH_MATRIX_HPP
