/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:tmat;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.vectorization;
import :traits;
import :vector;
import :matrix;
import :matrix_construct;
import <concepts>;
import <type_traits>;
import <utility>;

namespace eagine {
namespace math {

/// @brief Generic template for RxC dimensional matrices.
/// @ingroup math
export template <typename T, int C, int R, bool RM, bool V>
struct tmat : matrix<T, C, R, RM, V> {
    /// @brief The base matrix type.
    using base = matrix<T, C, R, RM, V>;

    /// @brief Default constructor. Constructs identity matrix.
    constexpr tmat() noexcept
      : base{identity<base>{}()} {}

    /// @brief Construction from a base matrix.
    constexpr tmat(const base& m) noexcept
      : base{m} {}

    /// @brief Construction from element data pointer and count.
    tmat(const T* d, const int n) noexcept
      : base{base::from(d, n)} {}

    /// @brief Construction from element data array.
    tmat(const T (&d)[C * R]) noexcept
      : base{base::from(d, C * R)} {}

private:
    template <std::convertible_to<T>... P>
    static auto _make(P&&... p) noexcept
        requires((sizeof...(P)) == (C * R))
    {
        T d[C * R] = {T(p)...};
        return base::from(d, C * R);
    }

public:
    template <std::convertible_to<T>... P>
    tmat(P&&... p) noexcept
        requires((sizeof...(P)) == (R * C))
      : base(_make(std::forward<P>(p)...)) {}

    template <typename... P>
    constexpr tmat(const vector<P, RM ? C : R, V>&... v) noexcept
        requires((sizeof...(P)) == (RM ? R : C))
      : base{{v._v...}} {}

    template <std::convertible_to<T> P, int M, int N>
    constexpr tmat(const matrix<P, M, N, RM, V>& m) noexcept
        requires((C <= M) and (R <= N))
      : base{base::from(m)} {}
};

export template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<math::tmat<T, C, R, RM, V>> : std::bool_constant<RM> {};

} // namespace math

export template <typename T, int C, int R, bool RM, bool V>
struct is_known_matrix_type<math::tmat<T, C, R, RM, V>> : std::is_scalar<T> {};

export template <typename T, int C, int R, bool RM, bool V>
struct canonical_compound_type<math::tmat<T, C, R, RM, V>>
  : std::type_identity<std::remove_cv_t<T[C][R]>>{};

export template <typename T, int C, int R, bool RM, bool V>
struct compound_view_maker<math::tmat<T, C, R, RM, V>> {
    auto operator()(const math::tmat<T, C, R, RM, V>& m) const noexcept {
        return vect::view < T, RM ? C : R, V > ::apply(m._v);
    }
};

export template <typename T, int C, int R, bool RM, bool V>
struct is_row_major<math::tmat<T, C, R, RM, V>> : std::bool_constant<RM> {};

} // namespace eagine

