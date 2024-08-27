/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:vector;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.simd;
import :traits;
import :scalar;

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

    /// @brief Creates a new zero vector instance.
    [[nodiscard]] static auto zero() noexcept {
        return vector{vect::fill<T, N, V>::apply(T(0))};
    }

    /// @brief Creates a zero vector instance with all elements set to @p v.
    [[nodiscard]] static auto fill(const T v) noexcept {
        return vector{vect::fill<T, N, V>::apply(v)};
    }

    /// @brief Creates an unit axis vector for the I-th dimension.
    /// @pre I < N
    template <int I>
    [[nodiscard]] static auto axis() noexcept {
        return vector{vect::axis<T, N, I, V>::apply(T(1))};
    }

    /// @brief Creates an axis vector for the I-th dimension with specified length.
    /// @pre I < N
    template <int I>
    [[nodiscard]] static auto axis(const T v) noexcept {
        return vector{vect::axis<T, N, I, V>::apply(v)};
    }

    /// @brief Creates an axis vector for the i-th dimension with specified length.
    /// @pre i < N
    [[nodiscard]] static auto axis(const int i, const T v) noexcept {
        assert(i < N);
        vector r = zero();
        r._v[i] = v;
        return r;
    }

    /// @brief Creates vector instance with the specified elements.
    template <std::convertible_to<T>... P>
    [[nodiscard]] static constexpr auto make(P&&... p) noexcept
        requires(sizeof...(P) == N)
    {
        return vector{T(std::forward<P>(p))...};
    }

    /// @brief Creates vector instance from vector of another dimension.
    /// @param d specifies the value for additional elements if M < N.
    template <typename P, int M, bool W>
    [[nodiscard]] static constexpr auto from(
      const vector<P, M, W>& v,
      const T d = T(0)) noexcept
        requires(not std::is_same_v<T, P> or (N != M) or (V != W))
    {
        return vector{vect::cast<P, M, W, T, N, V>::apply(v._v, d)};
    }

    /// @brief Creates vector instance from two other vectors.
    template <typename P, int M, bool W>
    [[nodiscard]] static constexpr auto from(
      const vector<P, M, W>& v,
      const vector<T, N - M, W>& u) noexcept {
        return vector{vect::cast<P, M, W, T, N, V>::apply(v._v, u._v)};
    }

    /// @brief Creates vector instance from data pointer and length.
    [[nodiscard]] static auto from(const T* dt, const span_size_t sz) noexcept {
        return vector{vect::from_array<T, N, V>::apply(dt, sz)};
    }

    /// @brief Creates vector instance from data pointer, length and additional value.
    [[nodiscard]] static auto from(
      const T* dt,
      const span_size_t sz,
      const T fv) noexcept {
        return vector{vect::from_saafv<T, N, V>::apply(dt, sz, fv)};
    }

    /// @brief Subscript operator.
    [[nodiscard]] constexpr auto operator[](const int pos) const noexcept {
        return _v[pos];
    }

    /// @brief Returns the x-coordinate value.
    /// @pre N >= 1
    [[nodiscard]] constexpr auto x() const noexcept -> T
        requires(N > 0)
    {
        return _v[0];
    }

    /// @brief Returns the y-coordinate value.
    /// @pre N >= 2
    [[nodiscard]] constexpr auto y() const noexcept -> T
        requires(N > 1)
    {
        return _v[1];
    }

    /// @brief Returns the z-coordinate value.
    /// @pre N >= 3
    [[nodiscard]] constexpr auto z() const noexcept -> T
        requires(N > 2)
    {
        return _v[2];
    }

    /// @brief Returns the w-coordinate value.
    /// @pre N >= 4
    [[nodiscard]] constexpr auto w() const noexcept -> T
        requires(N > 3)
    {
        return _v[3];
    }

    /// @brief Unary plus operator.
    [[nodiscard]] auto operator+() const noexcept -> vector {
        return *this;
    }

    /// @brief Unary minus operator.
    [[nodiscard]] auto operator-() const noexcept -> vector {
        return {-_v};
    }

    /// @brief Addition operator.
    [[nodiscard]] auto operator+(const vector& a) const noexcept -> vector {
        return {_v + a._v};
    }

    /// @brief Addition operator.
    auto operator+=(const vector& a) noexcept -> auto& {
        _v = _v + a._v;
        return *this;
    }

    /// @brief Subtraction operator.
    [[nodiscard]] auto operator-(const vector& a) const noexcept -> vector {
        return {_v - a._v};
    }

    /// @brief Subtraction operator.
    auto operator-=(const vector& a) noexcept -> auto& {
        _v = _v - a._v;
        return *this;
    }

    /// @brief Multiplication operator.
    [[nodiscard]] auto operator*(const vector& a) const noexcept -> vector {
        return {_v * a._v};
    }

    /// @brief Multiplication operator.
    auto operator*=(const vector& a) noexcept -> auto& {
        _v = _v * a._v;
        return *this;
    }

    /// @brief Multiplication operator.
    [[nodiscard]] auto operator*(const scalar_type& c) const noexcept
      -> vector {
        static_assert(scalar_type::is_vectorized::value);
        return {_v * c._v};
    }

    /// @brief Multiplication by scalar operator.
    auto operator*=(const scalar_type& c) noexcept -> auto& {
        static_assert(scalar_type::is_vectorized::value);
        _v = _v * c._v;
        return *this;
    }

    /// @brief Multiplication by constant operator.
    [[nodiscard]] auto operator*(const T c) const noexcept -> vector {
        return {_v * vect::fill<T, N, V>::apply(c)};
    }

    /// @brief Multiplication by constant operator.
    auto operator*=(const T c) noexcept -> auto& {
        _v = _v * vect::fill<T, N, V>::apply(c);
        return *this;
    }

    /// @brief Division operator.
    [[nodiscard]] auto operator/(const vector& a) const noexcept -> vector {
        return {vect::sdiv<T, N, V>::apply(_v, a._v)};
    }

    /// @brief Division operator.
    [[nodiscard]] auto operator/(const scalar_type& c) noexcept -> vector {
        static_assert(scalar_type::is_vectorized::value);
        return {vect::sdiv<T, N, V>::apply(_v, c._v)};
    }

    /// @brief Division by constant operator.
    [[nodiscard]] auto operator/(const T c) const noexcept -> vector {
        return {vect::sdiv<T, N, V>::apply(_v, vect::fill<T, N, V>::apply(c))};
    }

    /// @brief Returns the dimension of this vector.
    [[nodiscard]] constexpr auto dimension() const noexcept -> span_size_t {
        return span_size_t(N);
    }

    /// @brief Returns the magnitude of this vector.
    /// @see length
    [[nodiscard]] constexpr auto magnitude() const noexcept {
        if constexpr(vect::has_simd_data<T, N, V>::value) {
            return scalar_type{
              vect::sqrt<T, N, V>::apply(vect::hsum<T, N, V>::apply(_v * _v))};
        } else {
            using std::sqrt;
            return scalar_type{T(sqrt(vect::esum<T, N, V>::apply(_v * _v)))};
        }
    }

    /// @brief Returns the length of this vector.
    [[nodiscard]] constexpr auto length() const noexcept {
        return magnitude();
    }

    /// @brief Tests if this vector is zero-length.
    [[nodiscard]] constexpr auto is_zero() const noexcept -> bool {
        return vect::is_zero<T, N, V>::apply(_v);
    }

    /// @brief Returns a normalized copy of this vector.
    [[nodiscard]] constexpr auto normalized() const noexcept -> vector {
        const scalar_type l{length()};
        if constexpr(vect::has_simd_data<T, N, V>::value) {
            return vector{vect::sdiv<T, N, V>::apply(_v, l._v)};
        } else {
            return vector{
              vect::sdiv<T, N, V>::apply(_v, vect::fill<T, N, V>::apply(l._v))};
        }
    }

    /// @brief Returns the dot product of this vector and another vector.
    [[nodiscard]] constexpr auto dot(const vector& v) const noexcept {
        if constexpr(vect::has_simd_data<T, N, V>::value) {
            return scalar_type{vect::hsum<T, N, V>::apply(_v * v._v)};
        } else {
            return scalar_type{vect::esum<T, N, V>::apply(_v * v._v)};
        }
    }
};

/// @brief Multiplication by scalar operator.
/// @relates vector
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto operator*(
  const scalar<T, N, V>& c,
  const vector<T, N, V>& a) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{c._v * a._v};
}

/// @brief Multiplication by constant operator.
/// @relates vector
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto operator*(
  const T c,
  const vector<T, N, V>& a) noexcept {
    return vector<T, N, V>{a._v * vect::fill<T, N, V>::apply(c)};
}

/// @brief Scalar division operator.
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto operator/(
  const scalar<T, N, V>& c,
  const vector<T, N, V>& a) noexcept {
    static_assert(scalar<T, N, V>::is_vectorized::value);
    return vector<T, N, V>{vect::sdiv<T, N, V>::apply(c._v, a._v)};
}

/// @brief Constant division operator.
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto operator/(
  const T c,
  const vector<T, N, V>& a) noexcept {
    return vector<T, N, V>{
      vect::sdiv<T, N, V>::apply(vect::fill<T, N, V>::apply(c), a._v)};
}

/// @brief Vector dot product.
/// @ingroup math
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto dot(
  const vector<T, N, V>& a,
  const vector<T, N, V>& b) noexcept {
    return a.dot(b);
}

/// @brief Returns a vector perpendicular to argument.
/// @ingroup math
export template <typename T, bool V>
[[nodiscard]] constexpr auto perpendicular(const vector<T, 2, V>& a) noexcept {
    return vector<T, 2, V>{{-a._v[1], a._v[0]}};
}

/// @brief 3D vector cross product.
/// @ingroup math
export template <typename T, bool V>
[[nodiscard]] constexpr auto cross(
  const vector<T, 3, V>& a,
  const vector<T, 3, V>& b) noexcept {
    using _sh = vect::shuffle<T, 3, V>;
    return vector<T, 3, V>{
      _sh::apply(a._v, vect::shuffle_mask<1, 2, 0>{}) *
        _sh::apply(b._v, vect::shuffle_mask<2, 0, 1>{}) -
      _sh::apply(a._v, vect::shuffle_mask<2, 0, 1>{}) *
        _sh::apply(b._v, vect::shuffle_mask<1, 2, 0>{})};
}

/// @brief Returns the length of a vector.
/// @ingroup math
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto length(const vector<T, N, V>& a) noexcept {
    return a.length();
}

/// @brief Returns a normalized copy of the specified vector.
/// @ingroup math
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto normalized(const vector<T, N, V>& a) noexcept {
    return a.normalized();
}

/// @brief Returns the distance between two vectors.
/// @ingroup math
export template <typename T, int N, bool V>
[[nodiscard]] constexpr auto distance(
  const vector<T, N, V>& a,
  const vector<T, N, V>& b) noexcept {
    return length(a - b);
}

export template <std::size_t I, typename T, int N, bool V>
constexpr auto get(vector<T, N, V>& v) noexcept -> T& {
    return v._v[I];
}

export template <std::size_t I, typename T, int N, bool V>
constexpr auto get(const vector<T, N, V>& v) noexcept -> T {
    return v._v[I];
}
//------------------------------------------------------------------------------
/// @brief Generic template for N-dimensional vectors.
/// @ingroup math
export template <typename T, int N, bool V = true>
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
    template <std::convertible_to<T>... P>
    constexpr tvec(P&&... p) noexcept
        requires(sizeof...(P) == N)
      : base{base::make(std::forward<P>(p)...)} {}

    /// @brief Construction from vector of different dimensionality.
    template <typename P, int M, bool W>
    constexpr tvec(const vector<P, M, W>& v) noexcept
        requires(not std::is_same_v<P, T> or not(M == N))
      : base{base::from(v)} {}

    /// @brief Construction from vector of different dimensionality.
    template <typename P, int M, bool W>
    constexpr tvec(const vector<P, M, W>& v, const T d) noexcept
      : base{base::from(v, d)} {}

    /// @brief Construction from vector of different dimensionality.
    template <std::convertible_to<T> P, int M, bool W, std::convertible_to<T>... R>
    constexpr tvec(const vector<P, M, W>& v, R&&... r) noexcept
        requires((sizeof...(R) > 1) and (M + sizeof...(R) == N))
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

export template <typename T, int N, bool V>
struct is_known_vector_type<math::tvec<T, N, V>> : std::is_scalar<T> {};

export template <typename T, int N, bool V>
struct canonical_compound_type<math::tvec<T, N, V>>
  : std::type_identity<std::remove_cv_t<T[N]>> {};

export template <typename T, int N, bool V>
struct compound_view_maker<math::tvec<T, N, V>> {
    constexpr auto operator()(const math::tvec<T, N, V>& v) const noexcept {
        return vect::view<T, N, V>::apply(v._v);
    }
};

export template <typename T, int N, bool V>
struct flatten_traits<math::tvec<T, N, V>, T> {

    template <typename Ps, typename Ss>
    static constexpr auto required_size(
      const memory::basic_span<const math::tvec<T, N, V>, Ps, Ss> src) noexcept
      -> span_size_t {
        return src.size() * N;
    }

    template <typename Pd, typename Sd>
    static auto apply(
      const math::tvec<T, N, V>& src,
      memory::basic_span<T, Pd, Sd> dst) noexcept {
        assert(N <= dst.size());
        _do_apply(src._v, dst, std::make_index_sequence<std::size_t(N)>{});
        return skip(dst, N);
    }

private:
    template <typename Pd, typename Sd, std::size_t... I>
    static void _do_apply(
      const vect::data_t<T, N, V> src,
      memory::basic_span<T, Pd, Sd> dst,
      std::index_sequence<I...>) noexcept {
        ((dst[I] = src[I]), ...);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
  //
namespace std {

template <typename T, int N, bool V>
struct is_arithmetic<eagine::math::vector<T, N, V>> : true_type {};

template <typename T, int N, bool V>
struct is_arithmetic<eagine::math::tvec<T, N, V>> : true_type {};

template <typename T, int N, bool V>
struct tuple_size<eagine::math::vector<T, N, V>>
  : integral_constant<std::size_t, std::size_t(N)> {};

template <std::size_t I, typename T, int N, bool V>
struct tuple_element<I, eagine::math::vector<T, N, V>> : type_identity<T> {};

template <typename T, int N, bool V>
struct tuple_size<eagine::math::tvec<T, N, V>>
  : integral_constant<std::size_t, std::size_t(N)> {};

template <std::size_t I, typename T, int N, bool V>
struct tuple_element<I, eagine::math::tvec<T, N, V>> : type_identity<T> {};
//------------------------------------------------------------------------------
} // namespace std
