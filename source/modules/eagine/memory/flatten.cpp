/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:flatten;

import std;
import eagine.core.types;
import :span;

namespace eagine {

export template <typename Src, typename Dst>
struct flatten_traits;

namespace memory {
//------------------------------------------------------------------------------
export template <
  typename Ts,
  typename Ps,
  typename Ss,
  typename Td,
  typename Pd,
  typename Sd>
auto flatten(
  const basic_span<Ts, Ps, Ss> src,
  basic_span<Td, Pd, Sd> dst) noexcept -> basic_span<Td, Pd, Sd> {
    flatten_traits<std::remove_cv_t<Ts>, Td> traits{};
    assert(traits.required_size(src) <= dst.size());

    auto tmp = dst;
    for(auto& elem : src) {
        tmp = traits.apply(elem, tmp);
    }
    return head(dst, dst.size() - tmp.size());
}
//------------------------------------------------------------------------------
export template <typename Ts, typename Ps, typename Ss, typename Td, typename A>
auto flatten(const basic_span<Ts, Ps, Ss> src, std::vector<Td, A>& dst)
  -> auto& {
    flatten_traits<std::remove_cv_t<Ts>, Td> traits{};
    dst.resize(integer(traits.required_size(src)));
    flatten(src, cover(dst));
    return dst;
}
//------------------------------------------------------------------------------
} // namespace memory

export template <typename Ts, std::size_t N, typename Td>
struct flatten_traits<std::array<Ts, N>, Td> {
    static_assert(std::is_convertible_v<Ts, Td>);

    template <typename Ps, typename Ss>
    static constexpr auto required_size(
      const memory::basic_span<const std::array<Ts, N>, Ps, Ss> src) noexcept
      -> span_size_t {
        return src.size() * span_size(N);
    }

    template <typename Pd, typename Sd>
    static auto apply(
      const std::array<Ts, N>& src,
      memory::basic_span<Td, Pd, Sd> dst) noexcept {
        assert(span_size(N) <= dst.size());
        copy(view(src), dst);
        return skip(dst, span_size(N));
    }
};
//------------------------------------------------------------------------------
export template <typename T, std::convertible_to<T>... P>
struct flatten_traits<std::tuple<P...>, T> {

    template <typename Ps, typename Ss>
    static constexpr auto required_size(
      const memory::basic_span<const std::tuple<P...>, Ps, Ss> src) noexcept
      -> span_size_t {
        return src.size() * span_size(sizeof...(P));
    }

    template <typename Pd, typename Sd>
    static auto apply(
      const std::tuple<P...>& src,
      memory::basic_span<T, Pd, Sd> dst) noexcept {
        assert(span_size(sizeof...(P)) <= dst.size());
        _do_apply(src, dst, std::make_index_sequence<sizeof...(P)>{});
        return skip(dst, span_size(sizeof...(P)));
    }

private:
    template <typename Pd, typename Sd, std::size_t... I>
    static void _do_apply(
      const std::tuple<P...>& src,
      memory::basic_span<T, Pd, Sd> dst,
      std::index_sequence<I...>) noexcept {
        ((dst[I] = T(std::get<I>(src))), ...);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

