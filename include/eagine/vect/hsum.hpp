/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_VECT_HSUM_HPP
#define EAGINE_VECT_HSUM_HPP

#include "shuffle.hpp"

namespace eagine::vect {

template <typename T, int N, bool V>
struct hsum {
private:
    template <int... I>
    static auto _sh_apply(data_param_t<T, N, V> v) noexcept {
        return shuffle<T, N, V>::template apply<I...>(v);
    }

    template <int M, bool B>
    static auto _hlp(
      data_t<T, N, V> v,
      const int_constant<M>,
      const bool_constant<B>) noexcept -> data_t<T, N, V> {
        static_assert(M == N);

        for(int i = 1; i < N; ++i) {
            v[i] += v[i - 1];
        }
        for(int i = N - 1; i > 0; --i) {
            v[i - 1] = v[i];
        }
        return v;
    }

#if EAGINE_VECT_OPTS
    template <bool B>
    static constexpr auto
    _hlp(data_t<T, N, V> v, int_constant<1>, bool_constant<B>) noexcept {
        return v;
    }

    static constexpr auto
    _hlp(data_param_t<T, N, V> v, int_constant<2>, std::true_type) noexcept {
        return v + _sh_apply<1, 0>(v);
    }

    static constexpr auto
    _hlp3_1(data_param_t<T, N, V> t, data_param_t<T, N, V> v) noexcept {
        return t + _sh_apply<2, 2, 1>(v);
    }

    static constexpr auto
    _hlp(data_param_t<T, N, V> v, int_constant<3>, std::true_type) noexcept {
        return _hlp3_1(v + _sh_apply<1, 0, 0>(v), v);
    }

    static constexpr auto _hlp4_1(data_param_t<T, N, V> v) noexcept {
        return v + _sh_apply<2, 3, 0, 1>(v);
    }

    static constexpr auto
    _hlp(data_param_t<T, N, V> v, int_constant<4>, std::true_type) noexcept {
        return _hlp4_1(v + _sh_apply<1, 0, 3, 2>(v));
    }

    static constexpr auto _hlp8_1(data_param_t<T, N, V> v) noexcept {
        return v + _sh_apply<1, 0, 3, 2, 5, 4, 7, 6>(v);
    }

    static constexpr auto _hlp8_2(data_param_t<T, N, V> v) noexcept {
        return v + _sh_apply<2, 3, 0, 1, 6, 7, 4, 5>(v);
    }

    static constexpr auto _hlp8_3(data_param_t<T, N, V> v) noexcept {
        return v + _sh_apply<4, 5, 6, 7, 0, 1, 2, 3>(v);
    }

    static constexpr auto
    _hlp(data_param_t<T, N, V> v, int_constant<8>, std::true_type) noexcept {
        return _hlp8_3(_hlp8_2(_hlp8_1(v)));
    }
#endif
public:
    static auto apply(data_t<T, N, V> v) noexcept -> data_t<T, N, V> {
        return _hlp(v, int_constant<N>(), has_vect_data<T, N, V>());
    }
};

} // namespace eagine::vect

#endif // EAGINE_VECT_HSUM_HPP
