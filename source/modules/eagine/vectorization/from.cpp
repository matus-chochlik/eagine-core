/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.vectorization:from;

import eagine.core.types;
import :data;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct from_array {
    [[nodiscard]] static constexpr auto apply(
      const T* d,
      [[maybe_unused]] const span_size_t n) noexcept -> data_t<T, N, V> {
        assert(N <= int(n));
        data_t<T, N, V> r;
        for(int i = 0; i < N; ++i) {
            r[i] = d[i];
        }
        return r;
    }
};

export template <typename T, bool V>
struct from_array<T, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T*, const int) noexcept {
        return data_t<T, 0, V>{};
    }
};

export template <typename T, bool V>
struct from_array<T, 1, V> {
    [[nodiscard]] static constexpr auto apply(
      const T* d,
      const span_size_t n) noexcept {
        assert(1 <= n);
        return data_t<T, 1, V>{d[0]};
    }
};

export template <typename T, bool V>
struct from_array<T, 2, V> {
    [[nodiscard]] static constexpr auto apply(
      const T* d,
      const span_size_t n) noexcept {
        assert(2 <= n);
        return data_t<T, 2, V>{d[0], d[1]};
    }
};

export template <typename T, bool V>
struct from_array<T, 3, V> {
    [[nodiscard]] static auto apply(const T* d, const span_size_t n) noexcept {
        assert(3 <= n);
        return data_t<T, 3, V>{d[0], d[1], d[2]};
    }
};

export template <typename T, bool V>
struct from_array<T, 4, V> {
    [[nodiscard]] static auto apply(const T* d, const span_size_t n) noexcept {
        assert(4 <= n);
        return data_t<T, 4, V>{d[0], d[1], d[2], d[3]};
    }
};

export template <typename T, bool V>
struct from_array<T, 8, V> {
    [[nodiscard]] static auto apply(const T* d, const span_size_t n) noexcept {
        assert(8 <= n);
        return data_t<T, 8, V>{d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]};
    }
};

// from shorter array and fallback value
export template <typename T, int N, bool V>
struct from_saafv {
    [[nodiscard]] static constexpr auto apply(
      const T* d,
      const span_size_t n,
      T v) noexcept {
        data_t<T, N, V> r{};
        for(int i = 0; i < N and i < int(n); ++i) {
            r[i] = d[i];
        }
        for(int i = int(n); i < N; ++i) {
            r[i] = v;
        }
        return r;
    }
};

} // namespace eagine::vect
