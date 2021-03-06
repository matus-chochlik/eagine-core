/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:vector;

import eagine.core.concepts;
import eagine.core.types;
import eagine.core.vectorization;
import :traits;
import :scalar;
import <cmath>;
import <type_traits>;
import <utility>;

namespace eagine {
namespace math {
//------------------------------------------------------------------------------
/// @brief Basic N-dimensional vector implementation template.
/// @ingroup math
/// @see tvec
/// @note This is a base class, typically tvec should be used in client code.
export template <typename T, int N, bool V>
struct vector {
    using type = vector;

    /// @brief Related scalar type.
    using scalar_type = scalar<T, N, V>;

    /// @brief Element value type.
    using value_type = T;

    /// @brief Indicates if the implementation uses SIMD extensions.
    using is_vectorized = vect::has_simd_data<T, N, V>;

    using data_type = vect::data_t<T, N, V>;

    data_type _v;

    /// @brief vector function parameter type.
    using vector_param = const vector&;

    /// @brief scalar function parameter type.
    using scalar_param = const scalar_type&;

    /// @brief Creates a new zero vector instance.
    static auto zero() noexcept {
        return vector{vect::fill<T, N, V>::apply(T(0))};
    }

    /// @brief Creates a zero vector instance with all elements set to @p v.
    static auto fill(const T v) noexcept {
        return vector{vect::fill<T, N, V>::apply(v)};
    }

    /// @brief Creates an unit axis vector for the I-th dimension.
    /// @pre I < N
    template <int I>
    static auto axis() noexcept {
        return vector{vect::axis<T, N, I, V>::apply(T(1))};
    }

    /// @brief Creates an axis vector for the I-th dimension with specified length.
    /// @pre I < N
    template <int I>
    static auto axis(const T v) noexcept {
        return vector{vect::axis<T, N, I, V>::apply(v)};
    }

    /// @brief Creates an axis vector for the i-th dimension with specified length.
    /// @pre i < N
    static auto axis(const int i, const T v) noexcept {
        assert(i < N);
        vector r = zero();
        r._v[i] = v;
        return r;
    }

    template <typename P>
    static constexpr auto make(P&& p) noexcept
        requires((N == 1) && (std::is_convertible_v<P, T>))
    {
        return vector{{T(std::forward<P>(p))}};
    }

    /// @brief Creates vector instance with the specified elements.
    template <typename... P>
    static constexpr auto make(P&&... p) noexcept
        requires((N > 1) && (sizeof...(P) == N))
    {
        return vector{{T(std::forward<P>(p))...}};
    }

    /// @brief Creates vector instance from vector of another dimension.
    /// @param d specifies the value for additional elements if M < N.
    template <typename P, int M, bool W>
    static constexpr auto from(
      const vector<P, M, W>& v,
      const T d = T(0)) noexcept
        requires(!std::is_same_v<T, P> || (N != M) || (V != W))
    {
        return vector{vect::cast<P, M, W, T, N, V>::apply(v._v, d)};
    }

    /// @brief Creates vector instance from two other vectors.
    template <typename P, int M, bool W>
    static constexpr auto from(
      const vector<P, M, W>& v,
      const vector<T, N - M, W>& u) noexcept {
        return vector{vect::cast<P, M, W, T, N, V>::apply(v._v, u._v)};
    }

    /// @brief Creates vector instance from data pointer and length.
    static auto from(const T* dt, const span_size_t sz) noexcept {
        return vector{vect::from_array<T, N, V>::apply(dt, sz)};
    }

    /// @brief Creates vector instance from data pointer, length and additional value.
    static auto from(const T* dt, const span_size_t sz, const T fv) noexcept {
        return vector{vect::from_saafv<T, N, V>::apply(dt, sz, fv)};
    }

    /// @brief Subscript operator.
    constexpr auto operator[](const int pos) const noexcept {
        return _v[pos];
    }

    /// @brief Returns the x-coordinate value.
    /// @pre N >= 1
    template <int M = N>
    constexpr auto x() const noexcept -> T requires(M > 0) {
                                               static_assert(M == N);
                                               return _v[0];
                                           }

    /// @brief Returns the y-coordinate value.
    /// @pre N >= 2
    template <int M = N>
    constexpr auto y() const noexcept -> T requires(M > 1) {
                                               static_assert(M == N);
                                               return _v[1];
                                           }

    /// @brief Returns the z-coordinate value.
    /// @pre N >= 3
    template <int M = N>
    constexpr auto z() const noexcept -> T requires(M > 2) {
                                               static_assert(M == N);
                                               return _v[2];
                                           }

    /// @brief Returns the w-coordinate value.
    /// @pre N >= 4
    template <int M = N>
    constexpr auto w() const noexcept -> T requires(M > 3) {
                                               static_assert(M == N);
                                               return _v[3];
                                           }

    /// @brief Addition operator.
    auto
    operator+=(vector_param a) noexcept -> auto& {
        _v = _v + a._v;
        return *this;
    }

    /// @brief Subtraction operator.
    auto operator-=(vector_param a) noexcept -> auto& {
        _v = _v - a._v;
        return *this;
    }

    /// @brief Multiplication operator.
    auto operator*=(vector_param a) noexcept -> auto& {
        _v = _v * a._v;
        return *this;
    }

    /// @brief Multiplication by scalar operator.
    auto operator*=(scalar_param c) noexcept -> auto& {
        static_assert(scalar_type::is_vectorized::value);
        _v = _v * c._v;
        return *this;
    }

    /// @brief Multiplication by constant operator.
    auto operator*=(const T c) noexcept -> auto& {
        _v = _v * vect::fill<T, N, V>::apply(c);
        return *this;
    }
};

export template <typename T, int N, bool V>
using vector_param = const vector<T, N, V>&;

export template <typename T, int N, bool V>
using scalar_param = const scalar<T, N, V>&;

/// @brief Unary plus operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator+(vector<T, N, V> a) noexcept {
    return a;
}

/// @brief Negation operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator-(vector_param<T, N, V> a) noexcept {
    return vector<T, N, V>{-a._v};
}

/// @brief Addition operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator+(
  vector_param<T, N, V> a,
  vector_param<T, N, V> b) noexcept {
    return vector<T, N, V>{a._v + b._v};
}

/// @brief Subtraction operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator-(
  vector_param<T, N, V> a,
  vector_param<T, N, V> b) noexcept {
    return vector<T, N, V>{a._v - b._v};
}

/// @brief Multiplication operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator*(
  vector_param<T, N, V> a,
  vector_param<T, N, V> b) noexcept {
    return vector<T, N, V>{a._v * b._v};
}

/// @brief Multiplication by scalar operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator*(
  scalar_param<T, N, V> c,
  vector_param<T, N, V> a) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{c._v * a._v};
}

/// @brief Multiplication by scalar operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator*(
  vector_param<T, N, V> a,
  scalar_param<T, N, V> c) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{a._v * c._v};
}

/// @brief Multiplication by constant operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator*(const T c, vector_param<T, N, V> a) noexcept {
    return vector<T, N, V>{a._v * vect::fill<T, N, V>::apply(c)};
}

/// @brief Multiplication by constant operator.
/// @relates vector
export template <typename T, int N, bool V>
constexpr auto operator*(vector_param<T, N, V> a, const T c) noexcept {
    return vector<T, N, V>{a._v * vect::fill<T, N, V>::apply(c)};
}

/// @brief Division operator.
export template <typename T, int N, bool V>
constexpr auto operator/(
  vector_param<T, N, V> a,
  vector_param<T, N, V> b) noexcept {
    return vector<T, N, V>{vect::sdiv<T, N, V>::apply(a._v, b._v)};
}

/// @brief Scalar division operator.
export template <typename T, int N, bool V>
constexpr auto operator/(
  scalar_param<T, N, V> c,
  vector_param<T, N, V> a) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{vect::sdiv<T, N, V>::apply(c._v, a._v)};
}

/// @brief Division by scalar operator.
export template <typename T, int N, bool V>
constexpr auto operator/(
  vector_param<T, N, V> a,
  scalar_param<T, N, V> c) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{vect::sdiv<T, N, V>::apply(a._v, c._v)};
}

/// @brief Division by constant operator.
export template <typename T, int N, bool V>
constexpr auto operator/(vector_param<T, N, V> a, const T c) noexcept {
    return vector<T, N, V>{
      vect::sdiv<T, N, V>::apply(a._v, vect::fill<T, N, V>::apply(c))};
}

/// @brief Constant division operator.
export template <typename T, int N, bool V>
constexpr auto operator/(const T c, vector_param<T, N, V> a) noexcept {
    return vector<T, N, V>{
      vect::sdiv<T, N, V>::apply(vect::fill<T, N, V>::apply(c), a._v)};
}

/// @brief Returns the dimension of a vector.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto dimension(vector_param<T, N, V>) noexcept {
    return span_size_t(N);
}

/// @brief Tests if a vector has zero lenght.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto is_zero(vector_param<T, N, V> v) noexcept -> bool {
    return vect::is_zero<T, N, V>::apply(v._v);
}

/// @brief Vector dot product.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto dot(vector_param<T, N, V> a, vector_param<T, N, V> b) noexcept {
    if constexpr(vect::has_simd_data<T, N, V>::value) {
        return scalar<T, N, V>{vect::hsum<T, N, V>::apply(a._v * b._v)};
    } else {
        return scalar<T, N, V>{vect::esum<T, N, V>::apply(a._v * b._v)};
    }
}

/// @brief Returns a vector perpendicular to argument.
/// @ingroup math
export template <typename T, bool V>
constexpr auto perpendicular(vector_param<T, 2, V> a) noexcept {
    return vector<T, 2, V>{{-a._v[1], a._v[0]}};
}

/// @brief 3D vector cross product.
/// @ingroup math
export template <typename T, bool V>
constexpr auto cross(vector_param<T, 3, V> a, vector_param<T, 3, V> b) noexcept {
    using _sh = vect::shuffle<T, 3, V>;
    return vector<T, 3, V>{
      _sh::apply(a._v, vect::shuffle_mask<1, 2, 0>{}) *
        _sh::apply(b._v, vect::shuffle_mask<2, 0, 1>{}) -
      _sh::apply(a._v, vect::shuffle_mask<2, 0, 1>{}) *
        _sh::apply(b._v, vect::shuffle_mask<1, 2, 0>{})};
}

/// @brief Returns the magnitude of a vector. Same as length.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto magnitude(vector_param<T, N, V> a) noexcept {
    if constexpr(vect::has_simd_data<T, N, V>::value) {
        return scalar<T, N, V>{
          vect::sqrt<T, N, V>::apply(vect::hsum<T, N, V>::apply(a._v * a._v))};
    } else {
        using std::sqrt;
        return scalar<T, N, V>{
          T(sqrt(vect::esum<T, N, V>::apply(a._v * a._v)))};
    }
}

/// @brief Returns the length of a vector.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto length(vector_param<T, N, V> a) noexcept {
    return magnitude(a);
}

/// @brief Returns normalized argument.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto normalized(vector_param<T, N, V> a) noexcept {
    scalar<T, N, V> l = length(a);
    if constexpr(vect::has_simd_data<T, N, V>::value) {
        return vector<T, N, V>{vect::sdiv<T, N, V>::apply(a._v, l._v)};
    } else {
        return vector<T, N, V>{
          vect::sdiv<T, N, V>::apply(a._v, vect::fill<T, N, V>::apply(l._v))};
    }
}

/// @brief Returns the distance between two vectors.
/// @ingroup math
export template <typename T, int N, bool V>
constexpr auto distance(
  vector_param<T, N, V> a,
  vector_param<T, N, V> b) noexcept {
    return magnitude(a - b);
}

/// @brief Generic template for N-dimensional vectors.
/// @ingroup math
export template <typename T, int N, bool V>
struct tvec : vector<T, N, V> {
    /// @brief The base vector type.
    using base = vector<T, N, V>;

    /// @brief Default constructor. Constructs a zero vector.
    constexpr tvec() noexcept
      : base{base::zero()} {}

    /// @brief Constructor initializing all coordinates to @p v.
    constexpr tvec(T v) noexcept
      : base{base::fill(v)} {}

    /// @brief Construction from base vector.
    constexpr tvec(const base& v) noexcept
      : base{v} {}

    /// @brief Construction from native array.
    tvec(const T (&d)[N]) noexcept
      : base{base::from(static_cast<const T*>(d), N)} {}

    /// @brief Construction from coordinates.
    template <typename... P>
    constexpr tvec(P&&... p) noexcept
        requires((sizeof...(P) == N) && all_are_convertible_to<T, P...>::value)
    : base{base::make(std::forward<P>(p)...)} {}

    /// @brief Construction from vector of different dimensionality.
    template <typename P, int M, bool W>
    constexpr tvec(const vector<P, M, W>& v) noexcept
        requires(!std::is_same_v<P, T> || !(M == N))
    : base{base::from(v)} {}

    /// @brief Construction from vector of different dimensionality.
    template <typename P, int M, bool W>
    constexpr tvec(const vector<P, M, W>& v, const T d) noexcept
      : base{base::from(v, d)} {}

    /// @brief Construction from vector of different dimensionality.
    template <typename P, int M, bool W, typename... R>
    constexpr tvec(const vector<P, M, W>& v, R&&... r) noexcept
        requires(
          (sizeof...(R) > 1) && (M + sizeof...(R) == N) &&
          all_are_convertible_to<T, R...>::value)
    : base{base::from(v, vector<T, N - M, W>::make(std::forward<R>(r)...))} {}

    /// @brief Construction from a pair of vectors of different dimensionality.
    template <typename P, int M, bool W>
    constexpr tvec(
      const vector<P, M, W>& v,
      const vector<T, N - M, W>& w) noexcept
      : base{base::from(v, w)} {}
};
} // namespace math
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct is_known_vector_type<math::vector<T, N, V>> : std::is_scalar<T> {};

export template <typename T, int N, bool V>
struct canonical_compound_type<math::vector<T, N, V>>
  : std::type_identity<std::remove_cv_t<T[N]>> {};

export template <typename T, int N, bool V>
struct compound_view_maker<math::vector<T, N, V>> {
    constexpr auto operator()(const math::vector<T, N, V>& v) const noexcept {
        return vect::view<T, N, V>::apply(v._v);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
