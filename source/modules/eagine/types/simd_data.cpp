// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:simd_data;

import std;

namespace eagine::simd {
//------------------------------------------------------------------------------
export template <typename T, int N>
struct data_simd;

export template <typename T, int N>
struct _has_simd_data : std::false_type {};

#if defined(__clang__)

// uint8_t
export template <>
struct data_simd<std::uint8_t, 2> {
    using type __attribute__((vector_size(2))) = std::uint8_t;
};

export template <>
struct data_simd<std::uint8_t, 4> {
    using type __attribute__((vector_size(4))) = std::uint8_t;
};

export template <>
struct data_simd<std::uint8_t, 8> {
    using type __attribute__((vector_size(8))) = std::uint8_t;
};

export template <>
struct data_simd<std::uint8_t, 16> {
    using type __attribute__((vector_size(16))) = std::uint8_t;
};

export template <>
struct data_simd<std::uint8_t, 32> {
    using type __attribute__((vector_size(32))) = std::uint8_t;
};

// uint16_t
export template <>
struct data_simd<std::uint16_t, 4> {
    using type __attribute__((vector_size(8))) = std::uint16_t;
};

// uint32_t
export template <>
struct data_simd<std::uint32_t, 2> {
    using type __attribute__((vector_size(8))) = std::uint32_t;
};

export template <>
struct data_simd<std::uint32_t, 4> {
    using type __attribute__((vector_size(16))) = std::uint32_t;
};

// uint64_t
export template <>
struct data_simd<std::uint64_t, 2> {
    using type __attribute__((vector_size(16))) = std::uint64_t;
};

// uint64_t
export template <>
struct data_simd<std::uint64_t, 4> {
    using type __attribute__((vector_size(32))) = std::uint64_t;
};

// int8_t
export template <>
struct data_simd<std::int8_t, 8> {
    using type __attribute__((vector_size(8))) = std::int8_t;
};

// int16_t
export template <>
struct data_simd<std::int16_t, 4> {
    using type __attribute__((vector_size(8))) = std::int16_t;
};

// int32_t
export template <>
struct data_simd<std::int32_t, 2> {
    using type __attribute__((vector_size(8))) = std::int32_t;
};

export template <>
struct data_simd<std::int32_t, 4> {
    using type __attribute__((vector_size(16))) = std::int32_t;
};

// int64_t
export template <>
struct data_simd<std::int64_t, 2> {
    using type __attribute__((vector_size(16))) = std::int64_t;
};

export template <>
struct data_simd<std::int64_t, 4> {
    using type __attribute__((vector_size(32))) = std::int64_t;
};

// float
export template <>
struct data_simd<float, 2> {
    using type __attribute__((vector_size(8))) = float;
};

export template <>
struct data_simd<float, 3> {
    using type __attribute__((vector_size(16))) = float;
};

export template <>
struct data_simd<float, 4> {
    using type __attribute__((vector_size(16))) = float;
};

// double
export template <>
struct data_simd<double, 2> {
    using type __attribute__((vector_size(16))) = double;
};

export template <>
struct data_simd<double, 3> {
    using type __attribute__((vector_size(32))) = double;
};

export template <>
struct data_simd<double, 4> {
    using type __attribute__((vector_size(32))) = double;
};

#elif defined(__GNUC__)

export template <typename T, int N>
struct _gnuc_vec_data {
    using type __attribute__((vector_size(sizeof(T) * N))) = T;
};

export template <typename T>
struct data_simd<T, 2> : _gnuc_vec_data<T, 2> {};

export template <typename T>
struct data_simd<T, 3> : _gnuc_vec_data<T, 4> {};

export template <typename T>
struct data_simd<T, 4> : _gnuc_vec_data<T, 4> {};

export template <typename T>
struct data_simd<T, 8> : _gnuc_vec_data<T, 8> {};

export template <typename T>
struct data_simd<T, 16> : _gnuc_vec_data<T, 16> {};

#endif // platform

// has_vec_data<uint8_t>
export template <int N>
struct _has_simd_data<std::uint8_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      ((N == 2) or (N == 4) or (N == 8) or (N == 16)) or
#endif
#if defined(__MMX__) && __MMX__
      ((N == 2) or (N == 4) or (N == 8)) or
#endif
      false> {
};

// has_vec_data<int8_t>
export template <int N>
struct _has_simd_data<std::int8_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      ((N == 2) or (N == 4) or (N == 8) or (N == 16)) or
#endif
#if defined(__MMX__) && __MMX__
      ((N == 2) or (N == 4) or (N == 8)) or
#endif
      false> {
};

// has_vec_data<int16_t>
export template <int N>
struct _has_simd_data<std::int16_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      ((N == 2) or (N == 4) or (N == 8)) or
#endif
#if defined(__MMX__) && __MMX__
      ((N == 2) or (N == 4)) or
#endif
      false> {
};

// has_vec_data<int32_t>
export template <int N>
struct _has_simd_data<std::int32_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      ((N == 2) or (N == 4)) or
#endif
#if defined(__MMX__) && __MMX__
      (N == 2) or
#endif
      false> {
};

// has_vec_data<int64_t>
export template <int N>
struct _has_simd_data<std::int64_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      (N == 2) or
#endif
      false> {
};

// has_vec_data<int64_t>
export template <int N>
struct _has_simd_data<std::uint64_t, N>
  : std::bool_constant<
#if defined(__SSE2__) && __SSE2__
      (N == 2) or
#endif
      false> {
};

// has_vec_data<float>
export template <int N>
struct _has_simd_data<float, N>
  : std::bool_constant<
#if defined(__AVX__) && __AVX__
      ((N == 2) or (N == 3) or (N == 4)) or
#endif
#if defined(__SSE__) && __SSE__
      ((N == 2) or (N == 3) or (N == 4)) or
#endif
      false> {
};

// has_vec_data<double>
export template <int N>
struct _has_simd_data<double, N>
  : std::bool_constant<
#if defined(__AVX__) && __AVX__
      ((N == 2) or (N == 3) or (N == 4)) or
#endif
#if defined(__SSE2__) && __SSE2__
      (N == 2) or
#endif
      false> {
};
//------------------------------------------------------------------------------
export template <typename T, int N>
struct data_array;

export template <typename T, int N>
struct data_array_param {
    using type = const data_array<T, N>&;
};

export template <typename T, int N>
using data_array_param_t = typename data_array_param<T, N>::type;

export template <typename T, int N>
struct data_array {
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_move_assignable_v<T>);

    T _v[N]{};

    using type = data_array;
    using param_t = data_array_param_t<T, N>;

    constexpr auto operator<=>(const data_array&) const noexcept = default;
    constexpr auto operator==(const data_array&) const noexcept
      -> bool = default;

    [[nodiscard]] constexpr auto operator[](const int i) const noexcept -> T {
        return _v[i];
    }

    [[nodiscard]] constexpr auto operator[](const int i) noexcept -> T& {
        return _v[i];
    }

    constexpr auto operator+=(const data_array b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] += b[i];
        }
        return *this;
    }

    constexpr auto operator+=(const T b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] += b;
        }
        return *this;
    }

    constexpr auto operator-=(const data_array b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] -= b[i];
        }
        return *this;
    }

    constexpr auto operator-=(const T b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] -= b;
        }
        return *this;
    }

    constexpr auto operator*=(const data_array b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] *= b[i];
        }
        return *this;
    }

    constexpr auto operator*=(const T b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] *= b;
        }
        return *this;
    }

    constexpr auto operator/=(const data_array b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] /= b[i];
        }
        return *this;
    }

    constexpr auto operator/=(const T b) noexcept -> auto& {
        for(int i = 0; i < N; ++i) {
            _v[i] /= b;
        }
        return *this;
    }

    [[nodiscard]] constexpr auto operator+() const noexcept {
        return *this;
    }

    [[nodiscard]] constexpr auto operator-() const noexcept {
        data_array a{*this};
        for(int i = 0; i < N; ++i) {
            a._v[i] = -a._v[i];
        }
        return a;
    }

    [[nodiscard]] constexpr auto operator+(param_t that) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] + that._v[i];
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator+(const T v) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] + v;
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator-(param_t that) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] - that._v[i];
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator-(const T v) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] - v;
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator*(param_t that) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] * that._v[i];
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator*(const T v) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] * v;
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator/(param_t that) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] / that._v[i];
        }
        return c;
    }

    [[nodiscard]] constexpr auto operator/(const T v) const noexcept {
        data_array c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = _v[i] / v;
        }
        return c;
    }
};

export template <typename T>
struct data_array<T, 0U> {
    using type = data_array;

    auto operator[](const int i) const -> T;
};
//------------------------------------------------------------------------------
// has_simd_data
export template <typename T, int N, bool V>
using has_simd_data = std::bool_constant<(V and _has_simd_data<T, N>::value)>;

// data
export template <typename T, int N, bool V>
struct data
  : std::conditional_t<
      has_simd_data<T, N, V>::value,
      data_simd<T, N>,
      data_array<T, N>> {
    using value_type = T;
    static constexpr const int size = N;
};

export template <typename T, int N, bool V>
using data_t = typename data<T, N, V>::type;

// data_param
export template <typename T, int N, bool V>
struct data_param
  : std::conditional_t<
      has_simd_data<T, N, V>::value,
      const data_simd<T, N>,
      const data_array_param<T, N>> {};

// param
export template <typename Data>
struct param;

export template <typename T, int N, bool V>
using data_param_t = typename data_param<T, N, V>::type;

export template <typename T, int N, bool V>
struct param<data<T, N, V>> : data_param<T, N, V> {};
//------------------------------------------------------------------------------
// fill
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct fill {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept
      -> data_t<T, N, V> {
        if constexpr(N == 0) {
            return data_t<T, 0, V>{};
        } else if constexpr(N == 1) {
            return data_t<T, 1, V>{v};
        } else if constexpr(N == 2) {
            return data_t<T, 2, V>{v, v};
        } else if constexpr(N == 3) {
            return data_t<T, 3, V>{v, v, v};
        } else if constexpr(N == 4) {
            return data_t<T, 4, V>{v, v, v, v};
        } else if constexpr(N == 8) {
            return data_t<T, 8, V>{v, v, v, v, v, v, v, v};
        } else {
            data_t<T, N, V> r;
            for(int i = 0; i < N; ++i) {
                r[i] = v;
            }
            return r;
        }
    }
};
//------------------------------------------------------------------------------
// is_zero
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct is_zero {
    static constexpr auto _is_zero(std::floating_point auto v) noexcept
      -> bool {
        return not(v > T(0) or v < T(0));
    }

    static constexpr auto _is_zero(std::integral auto v) noexcept -> bool {
        return not v;
    }

    static constexpr auto apply(data_param_t<T, N, V> v) noexcept {
#if defined(__clang__)
        if constexpr(has_simd_data<T, N, V>::value) {
            if constexpr(std::is_integral_v<T>) {
                if constexpr(std::is_unsigned_v<T>) {
                    return _is_zero(__builtin_reduce_max(v));
                } else {
                    return _is_zero(
                      __builtin_reduce_max(__builtin_elementwise_abs(v)));
                }
            }
        }
#endif
        bool result = true;
        for(int i = 0; i < N; ++i) {
            result = result and _is_zero(v[i]);
        }
        return result;
    }
};
//------------------------------------------------------------------------------
// vector_compare
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct vector_compare {
    static constexpr auto apply(
      data_param_t<T, N, V> l,
      data_param_t<T, N, V> r) noexcept {
        if constexpr(has_simd_data<T, N, V>::value) {
            for(int i = 0; i < N; ++i) {
                if(l[i] < r[i]) {
                    return std::strong_ordering::less;
                } else if(l[i] > r[i]) {
                    return std::strong_ordering::greater;
                }
            }
            return std::strong_ordering::equal;
        } else {
            return l <=> r;
        }
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::simd
