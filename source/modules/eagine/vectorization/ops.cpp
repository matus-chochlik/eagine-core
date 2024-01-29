/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.vectorization:ops;

import std;
import eagine.core.types;
import eagine.core.memory;
import :data_simd;
import :data;

namespace eagine::vect {
//------------------------------------------------------------------------------
// is_zero
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct is_zero {
    [[nodiscard]] static auto apply(data_param_t<T, N, V> v) noexcept -> bool {
        bool result = true;
        for(int i = 0; i < N; ++i) {
            result &= not(v[i] > T(0) or v[i] < T(0));
        }
        return result;
    }
};
//------------------------------------------------------------------------------
// fill
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
// axis
//------------------------------------------------------------------------------
export template <typename T, int N, int I, bool V>
struct axis {
    [[nodiscard]] static auto apply(const T v) noexcept -> data_t<T, N, V> {
        data_t<T, N, V> r;
        for(int i = 0; i < N; ++i) {
            r[i] = (i == I) ? v : T(0);
        }
        return r;
    }
};

export template <typename T, bool V>
struct axis<T, 1, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 1, V>{v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 1, I, V> {
    [[nodiscard]] static constexpr auto apply(const T) noexcept {
        return data_t<T, 1, V>{0};
    }
};

export template <typename T, bool V>
struct axis<T, 2, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 2, V>{v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 2, 1, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 2, V>{T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 2, I, V> {
    [[nodiscard]] static constexpr auto apply(const T) noexcept {
        return data_t<T, 2, V>{T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{v, T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 1, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{T(0), v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 3, 2, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 3, V>{T(0), T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 3, I, V> {
    [[nodiscard]] static constexpr auto apply(const T) noexcept {
        return data_t<T, 3, V>{T(0), T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 0, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{v, T(0), T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 1, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), v, T(0), T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 2, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), T(0), v, T(0)};
    }
};

export template <typename T, bool V>
struct axis<T, 4, 3, V> {
    [[nodiscard]] static constexpr auto apply(const T v) noexcept {
        return data_t<T, 4, V>{T(0), T(0), T(0), v};
    }
};

export template <typename T, int I, bool V>
struct axis<T, 4, I, V> {
    [[nodiscard]] static constexpr auto apply(const T) noexcept {
        return data_t<T, 4, V>{T(0), T(0), T(0), T(0)};
    }
};
//------------------------------------------------------------------------------
// from_array
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
// cast
//------------------------------------------------------------------------------
export template <typename TF, int NF, bool VF, typename TT, int NT, bool VT>
struct cast;

export template <typename T, int N, bool V>
struct cast<T, N, V, T, N, V> {
    [[nodiscard]] static constexpr auto apply(
      const data_param_t<T, N, V> v,
      const T) noexcept -> data_t<T, N, V> {
        return v;
    }
};

export template <typename T, bool V>
struct cast<T, 4, V, T, 3, V> {
    [[nodiscard]] static constexpr auto apply(
      const data_param_t<T, 4, V> v,
      const T) noexcept -> data_t<T, 3, V> {
        return {v[0], v[1], v[2]};
    }
};

export template <typename T, bool V>
struct cast<T, 3, V, T, 4, V> {
    [[nodiscard]] static constexpr auto apply(
      const data_param_t<T, 3, V> v,
      const T d) noexcept -> data_t<T, 4, V> {
        return {v[0], v[1], v[2], d};
    }

    [[nodiscard]] static constexpr auto apply(
      const data_param_t<T, 3, V> v,
      const data_param_t<T, 1, V> d) noexcept -> data_t<T, 4, V> {
        return {v[0], v[1], v[2], d[0]};
    }
};

export template <typename TF, int NF, bool VF, typename TT, int NT, bool VT>
struct cast {
private:
    template <int... I>
    using _idx_seq = std::integer_sequence<int, I...>;

    template <int N>
    using _make_idx_seq = std::make_integer_sequence<int, N>;

    template <int... I, int... D>
    static constexpr auto _cast(
      const data_param_t<TF, NF, VF> v,
      const data_param_t<TT, sizeof...(D), VT> d,
      const _idx_seq<I...>,
      const _idx_seq<D...>) noexcept {
        return data_t<TT, NT, VT>{TT(v[I])..., TT(d[D])...};
    }

    template <int... I>
    static constexpr auto _cast(
      const data_param_t<TF, NF, VF> v,
      const data_param_t<TT, 0U, VT>,
      const _idx_seq<I...>,
      const _idx_seq<>) noexcept {
        return data_t<TT, NT, VT>{TT(v[I])...};
    }

public:
    [[nodiscard]] static constexpr auto apply(
      const data_param_t<TF, NF, VF> v,
      const data_param_t<TT, (NT > NF) ? NT - NF : 0, VT> d) noexcept {
        using is = _make_idx_seq<(NT > NF) ? NF : NT>;
        using ds = _make_idx_seq<(NT > NF) ? NT - NF : 0>;
        return _cast(v, d, is(), ds());
    }

    [[nodiscard]] static constexpr auto apply(
      const data_param_t<TF, NF, VF> v,
      const TT d) noexcept {
        return apply(v, fill < TT, (NT > NF) ? NT - NF : 0, VT > ::apply(d));
    }
};
//------------------------------------------------------------------------------
// shuffle
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
// view
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct view {
private:
    static auto _addr(const data_t<T, N, V>& d, const std::false_type) noexcept
      -> const T* {
        return static_cast<const T*>(d._v);
    }

    static auto _addr(const data_t<T, N, V>& d, const std::true_type) noexcept
      -> const T* {
        return reinterpret_cast<const T*>(&d);
    }

public:
    [[nodiscard]] static auto apply(const data_t<T, N, V>& d) noexcept
      -> span<const T> {
        static_assert(sizeof(T[N]) <= sizeof(data_t<T, N, V>));
        return {_addr(d, has_simd_data<T, N, V>()), N};
    }

    template <int M>
    [[nodiscard]] static auto apply(const data_t<T, N, V> (&d)[M]) noexcept
      -> span<const T> {
        static_assert(sizeof(T[N][M]) == sizeof(data_t<T, N, V>[M]));
        return {_addr(d[0], has_simd_data<T, N, V>()), N * M};
    }
};
//------------------------------------------------------------------------------
// abs
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct abs {
    [[nodiscard]] static auto apply(data_t<T, N, V> v) noexcept
      -> data_t<T, N, V> {
        for(int i = 0; i < N; ++i) {
            using std::abs;
            v[i] = abs(v[i]);
        }
        return v;
    }
};
//------------------------------------------------------------------------------
// diff
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct diff {
    [[nodiscard]] static auto apply(
      data_param_t<T, N, V> a,
      data_param_t<T, N, V> b) noexcept -> data_t<T, N, V> {
        return vect::abs<T, N, V>::apply(a - b);
    }
};
//------------------------------------------------------------------------------
// hsum
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct hsum {
private:
    using _sh = shuffle<T, N, V>;

    template <int M, bool B>
    static auto _hlp(
      data_t<T, N, V> v,
      const int_constant<M>,
      const std::bool_constant<B>) noexcept -> data_t<T, N, V> {
        static_assert(M == N);

        for(int i = 1; i < N; ++i) {
            v[i] += v[i - 1];
        }
        for(int i = N - 1; i > 0; --i) {
            v[i - 1] = v[i];
        }
        return v;
    }

    template <bool B>
    static constexpr auto _hlp(
      data_t<T, N, V> v,
      int_constant<1>,
      std::bool_constant<B>) noexcept {
        return v;
    }

    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<2>,
      std::true_type) noexcept {
        return v + _sh::apply(v, shuffle_mask<1, 0>{});
    }

    static constexpr auto _hlp3_1(
      data_param_t<T, N, V> t,
      data_param_t<T, N, V> v) noexcept {
        return t + _sh::apply(v, shuffle_mask<2, 2, 1>{});
    }

    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<3>,
      std::true_type) noexcept {
        return _hlp3_1(v + _sh::apply(v, shuffle_mask<1, 0, 0>{}), v);
    }

    static constexpr auto _hlp4_1(data_param_t<T, N, V> v) noexcept {
        return v + _sh::apply(v, shuffle_mask<2, 3, 0, 1>{});
    }

    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<4>,
      std::true_type) noexcept {
        return _hlp4_1(v + _sh::apply(v, shuffle_mask<1, 0, 3, 2>{}));
    }

    static constexpr auto _hlp8_1(data_param_t<T, N, V> v) noexcept {
        return v + _sh::apply(v, shuffle_mask<1, 0, 3, 2, 5, 4, 7, 6>{});
    }

    static constexpr auto _hlp8_2(data_param_t<T, N, V> v) noexcept {
        return v + _sh::apply(v, shuffle_mask<2, 3, 0, 1, 6, 7, 4, 5>{});
    }

    static constexpr auto _hlp8_3(data_param_t<T, N, V> v) noexcept {
        return v + _sh::apply(v, shuffle_mask<4, 5, 6, 7, 0, 1, 2, 3>{});
    }

    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<8>,
      std::true_type) noexcept {
        return _hlp8_3(_hlp8_2(_hlp8_1(v)));
    }

public:
    [[nodiscard]] static constexpr auto apply(data_param_t<T, N, V> v) noexcept
      -> data_t<T, N, V> {
        return _hlp(v, int_constant<N>{}, has_simd_data<T, N, V>{});
    }
};
//------------------------------------------------------------------------------
// esum
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct esum {
private:
    template <int M, bool B>
    static auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<M>,
      std::bool_constant<B>) noexcept -> T {
        static_assert(M == N);
        T r = T(0);

        for(int i = 0; i < N; ++i) {
            r += v[i];
        }
        return r;
    }

    template <bool B>
    static constexpr auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<1>,
      std::bool_constant<B>) noexcept -> T {
        return v[0];
    }

    template <int M>
    static auto _hlp(
      data_param_t<T, N, V> v,
      int_constant<M>,
      std::true_type) noexcept -> T {
        static_assert(M == N);
        return hsum<T, N, V>::apply(v)[0];
    }

public:
    [[nodiscard]] static auto apply(data_param_t<T, N, V> v) noexcept -> T {
        return _hlp(v, int_constant<N>{}, has_simd_data<T, N, V>{});
    }
};
//------------------------------------------------------------------------------
// sdiv
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct sdiv {
    static constexpr auto apply(
      data_param_t<T, N, V> a,
      data_param_t<T, N, V> b) noexcept -> data_t<T, N, V> {
        return a / b;
    }
};

#if defined(__GNUC__) || defined(__clang__)

export template <typename T, bool V>
struct sdiv<T, 3, V> {
private:
    static constexpr auto _hlp(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b,
      const std::true_type) noexcept -> data_t<T, 3, V> {
        return a / data_t<T, 3, V>{b[0], b[1], b[2], T(1)};
    }

    static constexpr auto _hlp(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b,
      const std::false_type) noexcept -> data_t<T, 3, V> {
        return a / b;
    }

public:
    [[nodiscard]] static constexpr auto apply(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b) noexcept -> data_t<T, 3, V> {
        return _hlp(a, b, has_simd_data<T, 3, V>{});
    }
};

#endif
//------------------------------------------------------------------------------
// sqrt
//------------------------------------------------------------------------------
export template <typename T, int N, bool V>
struct sqrt {
    [[nodiscard]] static constexpr auto apply(data_t<T, N, V> v) noexcept
      -> data_t<T, N, V> {
        for(int i = 0; i < N; ++i) {
            using std::sqrt;
            v[i] = T(sqrt(v[i]));
        }
        return v;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::vect
