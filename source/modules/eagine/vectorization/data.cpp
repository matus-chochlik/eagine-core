/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:data;

import :data_array;
import :data_simd;
import <type_traits>;

namespace eagine::vect {

// data
export template <typename T, int N, bool V>
struct data
  : std::conditional_t<
      _has_simd_data<T, N>::value && V,
      data_simd<T, N>,
      data_array<T, N>> {
    using value_type = T;
    static constexpr int size = N;
};

export template <typename T, int N, bool V>
using data_t = typename data<T, N, V>::type;

// has_simd_data
export template <typename T, int N, bool V>
struct has_simd_data : std::bool_constant<V && _has_simd_data<T, N>::value> {};

export template <typename T, int N, bool V>
using has_simd_data_t = typename has_simd_data<T, N, V>::type;

// data_param
export template <typename T, int N, bool V>
struct data_param
  : std::conditional_t<
      V && _has_simd_data<T, N>::value,
      const data_simd<T, N>,
      const data_array_param<T, N>> {};

// param
export template <typename Data>
struct param;

export template <typename T, int N, bool V>
using data_param_t = typename data_param<T, N, V>::type;

export template <typename T, int N, bool V>
struct param<data<T, N, V>> : data_param<T, N, V> {};

} // namespace eagine::vect
