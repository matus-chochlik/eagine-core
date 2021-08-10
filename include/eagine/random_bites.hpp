/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_RANDOM_BITES_HPP
#define EAGINE_RANDOM_BITES_HPP

#include "biteset.hpp"
#include "instead_of.hpp"
#include "int_constant.hpp"
#include <random>

namespace eagine {
//------------------------------------------------------------------------------
template <
  std::size_t B,
  typename T,
  std::size_t... I,
  typename Distribution,
  typename Engine>
auto make_random_biteset(
  std::index_sequence<I...>,
  Distribution& dist,
  Engine& engine) {
    using R = biteset<sizeof...(I), B, T>;
    return R{instead_of_t<size_constant<I>, Distribution&>(dist)(engine)...};
}
//------------------------------------------------------------------------------
template <std::size_t B, typename T, std::size_t... I, typename Engine>
auto make_random_biteset(
  const std::index_sequence<I...> idx_seq,
  const T min,
  const T max,
  Engine& engine) {
    std::uniform_int_distribution<T> dist(min, max);
    return make_random_biteset<B, T>(idx_seq, dist, engine);
}
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B, typename T, typename Engine>
auto random_biteset(const T min, const T max, Engine& engine)
  -> biteset<N, B, T> {
    return make_random_biteset<B, T>(
      std::make_index_sequence<N>(), min, max, engine);
}
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B, typename T>
auto random_biteset(const T min, const T max) -> biteset<N, B, T> {
    std::random_device engine;
    return make_random_biteset<B, T>(
      std::make_index_sequence<N>(), min, max, engine);
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_RANDOM_BITES_HPP
