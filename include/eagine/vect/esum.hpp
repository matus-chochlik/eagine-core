/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_VECT_ESUM_HPP
#define EAGINE_VECT_ESUM_HPP

#include "config.hpp"
#if EAGINE_VECT_OPTS
#include "hsum.hpp"
#else
#include "data.hpp"
#endif

namespace eagine::vect {

template <typename T, int N, bool V>
struct esum {
private:
    template <int M, bool B>
    static auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<M>,
      bool_constant<B>) noexcept -> T {
        static_assert(M == N);
        T r = T(0);

        for(int i = 0; i < N; ++i) {
            r += v[i];
        }
        return r;
    }

#if EAGINE_VECT_OPTS
    template <bool B>
    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<1>,
      bool_constant<B>) noexcept -> T {
        return v[0];
    }

    template <int M>
    static auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<M>,
      std::true_type) noexcept -> T {
        static_assert(M == N);
        return hsum<T, N, V>::apply(v)[N - 1];
    }
#endif
public:
    static auto apply(data_param_t<T, N, V> v) noexcept -> T {
        return _hlp(v, int_constant<N>(), has_vect_data<T, N, V>());
    }
};

} // namespace eagine::vect

#endif // EAGINE_VECT_ESUM_HPP
