/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:data_array;

import eagine.core.concepts;
import <cstdint>;
import <type_traits>;
import <utility>;

namespace eagine::vect {

export template <typename T, int N>
struct _ary_data;

export template <typename T, int N>
struct _ary_param {
    using type = const _ary_data<T, N>&;
};

export template <typename T, int N>
using _ary_param_t = typename _ary_param<T, N>::type;

export template <typename T, int N>
struct _ary_data {
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_move_assignable_v<T>);

    T _v[N]{};

    using type = _ary_data;
    using param_t = _ary_param_t<T, N>;

    constexpr _ary_data() = default;

    template <does_not_hide<_ary_data> P>
    constexpr _ary_data(P&& p) noexcept
        requires((N == 1) && (std::is_convertible_v<P, T>))
    : _v{T(std::forward<P>(p))} {}

    template <typename P1, typename P2, typename... Pn>
    constexpr _ary_data(P1&& p1, P2&& p2, Pn&&... pn) noexcept
        requires((sizeof...(Pn) + 2) == N)
    : _v{
        T(std::forward<P1>(p1)),
        T(std::forward<P2>(p2)),
        T(std::forward<Pn>(pn))...} {}

    constexpr auto operator[](const int i) const noexcept -> T {
        return _v[i];
    }

    inline auto operator[](const int i) noexcept -> T& {
        return _v[i];
    }

    constexpr auto operator+=(const _ary_data b) noexcept -> auto& {
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

    constexpr auto operator-=(const _ary_data b) noexcept -> auto& {
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

    constexpr auto operator*=(const _ary_data b) noexcept -> auto& {
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

    constexpr auto operator/=(const _ary_data b) noexcept -> auto& {
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

    friend constexpr auto operator+(const _ary_data a) noexcept {
        return a;
    }

    friend auto operator-(_ary_data a) noexcept {
        for(int i = 0; i < N; ++i) {
            a._v[i] = -a._v[i];
        }
        return a;
    }

    friend auto operator+(param_t a, param_t b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] + b._v[i];
        }
        return c;
    }

    friend auto operator+(param_t a, const T b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] + b;
        }
        return c;
    }

    friend auto operator-(param_t a, param_t b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] - b._v[i];
        }
        return c;
    }

    friend auto operator-(param_t a, const T b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] - b;
        }
        return c;
    }

    friend auto operator*(param_t a, param_t b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] * b._v[i];
        }
        return c;
    }

    friend auto operator*(param_t a, const T b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] * b;
        }
        return c;
    }

    friend auto operator/(param_t a, param_t b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] / b._v[i];
        }
        return c;
    }

    friend auto operator/(param_t a, const T b) noexcept {
        _ary_data c{};
        for(int i = 0; i < N; ++i) {
            c._v[i] = a._v[i] / b;
        }
        return c;
    }
};

export template <typename T>
struct _ary_data<T, 0U> {
    using type = _ary_data;

    auto operator[](const int i) const -> T;
};

} // namespace eagine::vect

