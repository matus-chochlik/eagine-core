/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:fill;

import :data;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct fill {
    [[nodiscard]] static auto apply(const T v) noexcept -> data_t<T, N, V> {
        data_t<T, N, V> r;
        for(int i = 0; i < N; ++i) {
            r[i] = v;
        }
        return r;
    }
};

export template <typename T, bool V>
struct fill<T, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T) noexcept {
        return data_t<T, 0, V>{};
    }
};

export template <typename T, bool V>
struct fill<T, 1, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 1, V>{v};
    }
};

export template <typename T, bool V>
struct fill<T, 2, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 2, V>{v, v};
    }
};

export template <typename T, bool V>
struct fill<T, 3, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{v, v, v};
    }
};

export template <typename T, bool V>
struct fill<T, 4, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{v, v, v, v};
    }
};

export template <typename T, bool V>
struct fill<T, 8, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 8, V>{v, v, v, v, v, v, v, v};
    }
};

} // namespace eagine::vect
