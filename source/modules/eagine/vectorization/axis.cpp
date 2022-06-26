/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:axis;

import :data;

namespace eagine::vect {

export template <typename T, int N, int I, bool V>
struct axis {
    static auto apply(const T v) noexcept -> data_t<T, N, V> {
        data_t<T, N, V> r;
        for(int i = 0; i < N; ++i) {
            r[i] = (i == I) ? v : T(0);
        }
        return r;
    }
};

export template <typename T, bool V>
struct axis<T, 1, 0, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 1, V>{v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 1, I, V> {
    static constexpr auto apply(const T) noexcept {
        return data_t<T, 1, V>{0};
    }
};

export template <typename T, bool V>
struct axis<T, 2, 0, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 2, V>{v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 2, 1, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 2, V>{T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 2, I, V> {
    static constexpr auto apply(const T) noexcept {
        return data_t<T, 2, V>{T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 0, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{v, T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 1, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{T(0), v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 2, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{T(0), T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 3, I, V> {
    static constexpr auto apply(const T) noexcept {
        return data_t<T, 3, V>{T(0), T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 0, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{v, T(0), T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 1, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), v, T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 2, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), T(0), v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 3, V> {
    static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), T(0), T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 4, I, V> {
    static constexpr auto apply(const T) noexcept {
        return data_t<T, 4, V>{T(0), T(0), T(0), T(0)};
    }
};

} // namespace eagine::vect
