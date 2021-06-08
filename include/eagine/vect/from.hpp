/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_VECT_FROM_HPP
#define EAGINE_VECT_FROM_HPP

#include "../assert.hpp"
#include "../maybe_unused.hpp"
#include "../types.hpp"
#include "data.hpp"

namespace eagine::vect {

template <typename T, int N, bool V>
struct from_array {
    static auto apply(const T* d, span_size_t n) noexcept -> data_t<T, N, V> {
        EAGINE_ASSERT(N <= int(n));
        EAGINE_MAYBE_UNUSED(n);
        data_t<T, N, V> r;
        for(int i = 0; i < N; ++i) {
            r[i] = d[i];
        }
        return r;
    }
};

#if EAGINE_VECT_OPTS

template <typename T, bool V>
struct from_array<T, 0, V> {
    static constexpr auto apply(const T*, int) noexcept {
        return data_t<T, 0, V>{};
    }
};

template <typename T, bool V>
struct from_array<T, 1, V> {
    static auto apply(const T* d, span_size_t n) noexcept {
        EAGINE_ASSERT(1 <= n);
        EAGINE_MAYBE_UNUSED(n);
        return data_t<T, 1, V>{d[0]};
    }
};

template <typename T, bool V>
struct from_array<T, 2, V> {
    static auto apply(const T* d, span_size_t n) noexcept {
        EAGINE_ASSERT(2 <= n);
        EAGINE_MAYBE_UNUSED(n);
        return data_t<T, 2, V>{d[0], d[1]};
    }
};

template <typename T, bool V>
struct from_array<T, 3, V> {
    static auto apply(const T* d, span_size_t n) noexcept {
        EAGINE_ASSERT(3 <= n);
        EAGINE_MAYBE_UNUSED(n);
        return data_t<T, 3, V>{d[0], d[1], d[2]};
    }
};

template <typename T, bool V>
struct from_array<T, 4, V> {
    static auto apply(const T* d, span_size_t n) noexcept {
        EAGINE_ASSERT(4 <= n);
        EAGINE_MAYBE_UNUSED(n);
        return data_t<T, 4, V>{d[0], d[1], d[2], d[3]};
    }
};

template <typename T, bool V>
struct from_array<T, 8, V> {
    static auto apply(const T* d, span_size_t n) noexcept {
        EAGINE_ASSERT(8 <= n);
        EAGINE_MAYBE_UNUSED(n);
        return data_t<T, 8, V>{d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]};
    }
};

#endif

// from shorter array and fallback value
template <typename T, int N, bool V>
struct from_saafv {
    static auto apply(const T* d, span_size_t n, T v) noexcept {
        data_t<T, N, V> r = {};
        for(int i = 0; i < N && i < int(n); ++i) {
            r[i] = d[i];
        }
        for(int i = int(n); i < N; ++i) {
            r[i] = v;
        }
        return r;
    }
};

} // namespace eagine::vect

#endif // EAGINE_VECT_FROM_HPP
