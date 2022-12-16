/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:shuffle;

import eagine.core.types;
import :data_simd;

import :data;
import <type_traits>;

namespace eagine::vect {

export template <int... I>
struct shuffle_mask {};

#if defined(__GNUC__) && !defined(__clang__)
export template <typename T, int N, bool V>
struct mask : _vec_data<T, N> {};

export template <int N, bool V>
struct mask<float, N, V> : _vec_data<std::int32_t, N> {};

export template <int N, bool V>
struct mask<double, N, V> : _vec_data<std::int64_t, N> {};

export template <typename T, int N, bool V>
using mask_t = typename mask<T, N, V>::type;
#endif

export template <typename T, int N, bool V>
struct shuffle {
private:
    template <int... I>
    static constexpr auto _do_apply(
      data_param_t<T, N, V> v,
      const shuffle_mask<I...>,
      const std::false_type) noexcept {
        return data_t<T, N, V>{v[I]...};
    }

    template <int... I>
    static constexpr auto _do_apply(
      data_param_t<T, N, V> v,
      const shuffle_mask<I...>,
      const std::true_type) noexcept -> data_t<T, N, V> {
#if EAGINE_USE_SIMD && defined(__clang__)
        // NOLINTNEXTLINE(hicpp-vararg)
        return data_t<T, N, V>(__builtin_shufflevector(v, v, I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v, _mT{I...});
#else
        return _do_apply(v, shuffle_mask<I...>{}, std::false_type{});
#endif
    }

    template <int I0, int I1, int I2>
    static constexpr auto _do_apply(
      data_param_t<T, N, V> v,
      const shuffle_mask<I0, I1, I2>,
      const std::true_type sel) noexcept -> data_t<T, N, V> {
        return _do_apply(v, shuffle_mask<I0, I1, I2, 0>{}, sel);
    }

public:
    template <int... I>
    [[nodiscard]] static constexpr auto apply(
      data_param_t<T, N, V> v,
      const shuffle_mask<I...> m) noexcept {
        return _do_apply(v, m, has_simd_data<T, N, V>{});
    }
};

export template <typename T, int N, bool V>
struct shuffle2 {
private:
    template <int M, int... I>
    static auto _do_apply(
      data_param_t<T, N, V> v1,
      data_param_t<T, N, V> v2,
      const shuffle_mask<I...>,
      const int_constant<M>,
      const std::false_type) noexcept {
        return data_t<T, N, V>{
          I < 0 ? T(0) : (int(I) < N ? v1[int(I)] : v2[int(I) % N])...};
    }

    template <int M, int... I>
    static auto _do_apply(
      data_param_t<T, N, V> v1,
      data_param_t<T, N, V> v2,
      const shuffle_mask<I...>,
      const int_constant<M>,
      const std::true_type) noexcept -> data_t<T, N, V> {
#if EAGINE_USE_SIMD && defined(__clang__)
        // NOLINTNEXTLINE(hicpp-vararg)
        return data_t<T, N, V>(__builtin_shufflevector(v1, v2, I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v1, v2, _mT{I...});
#else
        return _do_apply(
          v1, v2, shuffle_mask<I...>{}, int_constant<M>{}, std::false_type{});
#endif
    }

    template <int... I>
    static auto _do_apply(
      data_param_t<T, N, V> v1,
      data_param_t<T, N, V> v2,
      const shuffle_mask<I...>,
      const int_constant<3U>,
      const std::true_type) noexcept -> data_t<T, N, V> {
#if EAGINE_USE_SIMD && defined(__clang__)
        return data_t<T, N, V>(
          // NOLINTNEXTLINE(hicpp-vararg)
          __builtin_shufflevector(v1, v2, I >= 3 ? I + 1 : I...));
#elif EAGINE_USE_SIMD && defined(__GNUC__)
        using _mT = mask_t<T, N, V>;
        // NOLINTNEXTLINE(hicpp-vararg)
        return __builtin_shuffle(v1, v2, _mT{(I >= 3 ? I + 1 : I)...});
#else
        return _do_apply(
          v1, v2, shuffle_mask<I...>{}, int_constant<3>{}, std::false_type{});
#endif
    }

public:
    template <int... I>
    [[nodiscard]] static auto apply(
      data_param_t<T, N, V> v1,
      data_param_t<T, N, V> v2,
      const shuffle_mask<I...> m) noexcept {
        return _do_apply(
          v1, v2, m, int_constant<N>{}, has_simd_data<T, N, V>{});
    }
};

} // namespace eagine::vect
