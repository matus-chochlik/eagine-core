/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:data_simd;

import std;
import eagine.core.types;

namespace eagine::vect {

export template <typename T, int N>
struct data_simd;

export template <typename T, int N>
struct _has_simd_data : std::false_type {};

#if defined(__clang__)

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
struct data_simd<std::uint64_t, 2> {
    using type __attribute__((vector_size(16))) = std::uint64_t;
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

#endif // platform

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
      ((N == 2) or (N == 4)) or
#endif
#if defined(__SSE2__) && __SSE2__
      (N == 2) or
#endif
      false> {
};

} // namespace eagine::vect
