/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:data_array;

import eagine.core.concepts;
import std;

namespace eagine::vect {

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

} // namespace eagine::vect

