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
#include "result.hpp"
#include <utility>

namespace eagine::c_api {

template <typename T>
struct function_traits;

template <typename Api, typename Wrapper>
struct function_traits<Wrapper Api::*> {
    using api_type = Api;
    using wrapper_type = Wrapper;
    using signature = typename Wrapper::signature;
};

template <auto ptr>
using function_api_t = typename function_traits<decltype(ptr)>::api_type;

template <auto ptr>
using function_wrapper_t =
  typename function_traits<decltype(ptr)>::wrapper_type;

template <auto ptr>
using function_signature_t = typename function_traits<decltype(ptr)>::signature;

template <
  typename Api,
  typename Wrapper,
  Wrapper Api::*function,
  typename CSignature = typename Wrapper::signature,
  typename CppSignature = CSignature,
  typename Map = trivial_map>
class basic_adapted_function;

template <
  typename Api,
  typename Wrapper,
  Wrapper Api::*function,
  typename CRV,
  typename... CParam,
  typename CppRV,
  typename... CppParam,
  typename Map>
class basic_adapted_function<
  Api,
  Wrapper,
  function,
  CRV(CParam...),
  CppRV(CppParam...),
  Map> {
    template <std::size_t... I>
    constexpr auto _call(CppParam... param, std::index_sequence<I...>)
      const noexcept {
        using Ftw = function_traits<Wrapper>;
        const Map map{};
        return map(
          size_constant<0>{},
          Ftw::call(_api.*function, map(size_constant<I + 1>{}, param...)...),
          param...);
    }

    Api& _api;

public:
    basic_adapted_function(Api& api) noexcept
      : _api{api} {}

    explicit constexpr operator bool() const noexcept {
        return bool(_api.*function);
    }

    constexpr auto operator()(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return _call(param..., S{});
    }
};

template <
  auto function,
  typename CppSignature = function_signature_t<function>,
  typename Map = trivial_map>
using adapted_function = basic_adapted_function<
  function_api_t<function>,
  function_wrapper_t<function>,
  function,
  function_signature_t<function>,
  CppSignature,
  Map>;

} // namespace eagine::c_api

#endif

