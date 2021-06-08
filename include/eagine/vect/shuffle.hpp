/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_VECT_SHUFFLE_HPP
#define EAGINE_VECT_SHUFFLE_HPP

#include "data.hpp"

namespace eagine::vect {

template <int... I>
struct shuffle_mask {};

#if EAGINE_USE_SIMD && defined(__GNUC__) && !defined(__clang__)
template <typename T, int N, bool V>
struct mask : _vec_data<T, N> {};

template <int N, bool V>
struct mask<float, N, V> : _vec_data<std::int32_t, N> {};

template <int N, bool V>
struct mask<double, N, V> : _vec_data<std::int64_t, N> {};

template <typename T, int N, bool V>
using mask_t = typename mask<T, N, V>::type;
#endif

template <typename T, int N, bool V>
struct shuffle {
private:
    using _dT = data_t<T, N, V>;
    using _dpT = data_param_t<T, N, V>;

    template <int... I>
    static constexpr auto
    _do_apply(_dpT v, shuffle_mask<I...>, std::false_type) noexcept {
        return data_t<T, N, V>{v[I]...};
    }

    template <int... I>
    static constexpr auto
    _do_apply(_dpT v, shuffle_mask<I...>, std::true_type) noexcept -> _dT {
#if EAGINE_USE_SIMD && defined(__clang__)
        // NOLINTNEXTLINE(hicpp-vararg)
        return _dT(__builtin_shufflevector(v, v, I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v, _mT{I...});
#else
        return _do_apply(v, shuffle_mask<I...>(), std::false_type());
#endif
    }

public:
    template <int... I>
    static constexpr auto apply(_dpT v, shuffle_mask<I...> m = {}) noexcept {
        return _do_apply(v, m, has_vect_data<T, N, V>());
    }
};

template <typename T, int N, bool V>
struct shuffle2 {
private:
    using _dT = data_t<T, N, V>;
    using _dpT = data_param_t<T, N, V>;

    template <int U>
    using _int = int_constant<U>;

    template <int M, int... I>
    static auto _do_apply(
      _dpT v1,
      _dpT v2,
      shuffle_mask<I...>,
      _int<M>,
      std::false_type) noexcept {
        return data_t<T, N, V>{
          I < 0 ? T(0) : (int(I) < N ? v1[int(I)] : v2[int(I) % N])...};
    }

    template <int M, int... I>
    static auto _do_apply(
      _dpT v1,
      _dpT v2,
      shuffle_mask<I...>,
      _int<M>,
      std::true_type) noexcept -> _dT {
#if EAGINE_USE_SIMD && defined(__clang__)
        // NOLINTNEXTLINE(hicpp-vararg)
        return _dT(__builtin_shufflevector(v1, v2, I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v1, v2, _mT{I...});
#else
        return _do_apply(
          v1, v2, shuffle_mask<I...>(), _int<M>(), std::false_type());
#endif
    }

    template <int... I>
    static auto _do_apply(
      _dpT v1,
      _dpT v2,
      shuffle_mask<I...>,
      _int<3U>,
      std::true_type) noexcept -> _dT {
#if EAGINE_USE_SIMD && defined(__clang__)
        // NOLINTNEXTLINE(hicpp-vararg)
        return _dT(__builtin_shufflevector(v1, v2, I >= 3 ? I + 1 : I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v1, v2, _mT{(I >= 3 ? I + 1 : I)...});
#else
        return _do_apply(
          v1, v2, shuffle_mask<I...>(), _int<3>(), std::false_type());
#endif
    }

public:
    template <int... I>
    static auto apply(_dpT v1, _dpT v2, shuffle_mask<I...> m = {}) noexcept {
        return _do_apply(v1, v2, m, _int<N>(), has_vect_data<T, N, V>());
    }
};

} // namespace eagine::vect

#endif // EAGINE_VECT_SHUFFLE_HPP
