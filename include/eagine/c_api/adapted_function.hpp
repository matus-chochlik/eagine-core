/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_ADAPTED_FUNCION_HPP
#define EAGINE_C_API_ADAPTED_FUNCION_HPP

#include "../scope_exit.hpp"
#include "function.hpp"
#include "parameter_map.hpp"
#include "result.hpp"
#include <utility>

namespace eagine::c_api {

template <typename T>
struct method_traits;

template <typename Api, typename Wrapper>
struct method_traits<Wrapper Api::*> {
    using api_type = Api;
    using wrapper_type = Wrapper;
    using signature = typename Wrapper::signature;
};

template <auto ptr>
using method_api_t = typename method_traits<decltype(ptr)>::api_type;

template <auto ptr>
using method_wrapper_t = typename method_traits<decltype(ptr)>::wrapper_type;

template <auto ptr>
using method_signature_t = typename method_traits<decltype(ptr)>::signature;

template <
  typename Api,
  auto method,
  typename CSignature,
  typename CppSignature = CSignature,
  typename RvMap = trivial_map,
  typename ArgMap = trivial_map>
class basic_adapted_function;

template <
  typename Api,
  auto method,
  typename CRV,
  typename... CParam,
  typename CppRV,
  typename... CppParam,
  typename RvMap,
  typename ArgMap>
class basic_adapted_function<
  Api,
  method,
  CRV(CParam...),
  CppRV(CppParam...),
  RvMap,
  ArgMap> {
    using Ftw = function_traits<method_wrapper_t<method>>;

    template <std::size_t... I>
    constexpr auto _call(CppParam... param, std::index_sequence<I...>)
      const noexcept requires(std::is_same_v<RvMap, ArgMap>) {
        ArgMap map{};
        return Ftw::api_traits::check_result(
          _api,
          map(
            size_constant<0>{},
            Ftw::call(
              _api.*method, map(size_constant<I + 1>{}, 0, param...)...),
            param...));
    }

    template <std::size_t... I>
    constexpr auto _call(CppParam... param, std::index_sequence<I...>)
      const noexcept requires(!std::is_same_v<RvMap, ArgMap>) {
        ArgMap map{};
        return Ftw::api_traits::check_result(
          _api,
          RvMap{}(
            size_constant<0>{},
            Ftw::call(
              _api.*method, map(size_constant<I + 1>{}, 0, param...)...),
            param...));
    }

    template <std::size_t... I>
    constexpr auto _lazy(CppParam... param, std::index_sequence<I...>)
      const noexcept {
        ArgMap map{};
        return
          [&api{_api}, ... args{map(size_constant<I + 1>{}, 0, param...)}]() {
              Ftw::call(api.*method, args...);
          };
    }

public:
    basic_adapted_function(Api& api) noexcept
      : _api{api} {}

    constexpr auto underlying() const noexcept -> decltype(auto) {
        return _api.*method;
    }

    explicit constexpr operator bool() const noexcept {
        return bool(underlying());
    }

    constexpr auto operator()(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return _call(param..., S{});
    }

    auto raii(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return eagine::finally(_lazy(param..., S{}));
    }

    constexpr auto api() const noexcept -> const auto& {
        return _api;
    }

    constexpr auto api() noexcept -> auto& {
        return _api;
    }

private:
    Api& _api;
};

template <
  auto method,
  typename CppSignature = method_signature_t<method>,
  typename RvMap = make_map_t<method_signature_t<method>, CppSignature>,
  typename ArgMap = make_map_t<method_signature_t<method>, CppSignature>>
struct adapted_function
  : basic_adapted_function<
      method_api_t<method>,
      method,
      method_signature_t<method>,
      CppSignature,
      RvMap,
      ArgMap> {
    adapted_function(method_api_t<method>& api)
      : basic_adapted_function<
          method_api_t<method>,
          method,
          method_signature_t<method>,
          CppSignature,
          RvMap,
          ArgMap>{api} {}
};

template <
  typename Api,
  typename ApiTraits,
  typename Tag,
  typename CSignature,
  typename CppSignature,
  typename RvMap,
  typename ArgMap,
  function_ptr<ApiTraits, Tag, CSignature> function,
  bool isAvailable,
  bool isStatic>
class combined_function {
public:
    combined_function(string_view name, ApiTraits& api_traits, Api& api)
      : _basic{name, api_traits, api}
      , _adapted{api} {}

    constexpr explicit operator bool() const noexcept {
        return bool(_basic);
    }

private:
    opt_function<ApiTraits, Tag, CSignature, function, isAvailable, isStatic>
      _basic;
    basic_adapted_function<
      Api,
      &combined_function::_basic,
      CSignature,
      CppSignature,
      RvMap,
      ArgMap>
      _adapted;
};

} // namespace eagine::c_api

#endif

