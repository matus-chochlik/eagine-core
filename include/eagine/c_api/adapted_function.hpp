/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_ADAPTED_FUNCION_HPP
#define EAGINE_C_API_ADAPTED_FUNCION_HPP

#include "parameter_map.hpp"
#include <utility>

namespace eagine::c_api {

template <
  typename Api,
  typename WrapperType,
  WrapperType Api::*function,
  typename CSignature = typename WrapperType::signature,
  typename CppSignature = CSignature,
  typename Map = trivial_map>
class adapted_function;

template <
  typename Api,
  typename WrapperType,
  WrapperType Api::*function,
  typename CRV,
  typename... CParam,
  typename CppRV,
  typename... CppParam,
  typename Map>
class adapted_function<
  Api,
  WrapperType,
  function,
  CRV(CParam...),
  CppRV(CppParam...),
  Map> {
    template <std::size_t... I>
    constexpr auto _call(CppParam... param, std::index_sequence<I...>)
      const noexcept {
        const Map map{};
        return map(
          size_constant<0>{},
          (_api.*function)(map(size_constant<I + 1>{}, param...)...),
          param...);
    }

    Api& _api;

public:
    adapted_function(Api& api) noexcept
      : _api{api} {}

    constexpr auto operator()(CppParam... param) const noexcept {
        return _call(param..., std::make_index_sequence<sizeof...(CParam)>{});
    }
};

} // namespace eagine::c_api

#endif

