/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:cast;

import :data;
import :fill;
import <utility>;

namespace eagine::vect {

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

} // namespace eagine::vect
