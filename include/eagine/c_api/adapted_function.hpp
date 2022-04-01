/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_ADAPTED_FUNCTION_HPP
#define EAGINE_C_API_ADAPTED_FUNCTION_HPP

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

template <typename T>
struct adapted_function_raii_parameter {
    using type = T;
};

template <typename T>
using adapted_function_raii_parameter_t =
  typename adapted_function_raii_parameter<T>::type;

template <typename Tag, typename Handle, Handle invalid>
struct adapted_function_raii_parameter<basic_owned_handle<Tag, Handle, invalid>> {
    using type = basic_owned_handle<Tag, Handle, invalid>&;
};

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
    constexpr auto _call(std::index_sequence<I...>, CppParam... param)
      const noexcept requires(std::is_same_v<RvMap, ArgMap>) {
        ArgMap map{};
        return _api.check_result(
          map(
            size_constant<0>{},
            Ftw::call(
              _api.*method, map(size_constant<I + 1>{}, 0, param...)...),
            param...),
          param...);
    }

    template <std::size_t... I>
    constexpr auto _call(std::index_sequence<I...>, CppParam... param)
      const noexcept requires(!std::is_same_v<RvMap, ArgMap>) {
        ArgMap map{};
        return _api.check_result(
          RvMap{}(
            size_constant<0>{},
            Ftw::call(
              _api.*method, map(size_constant<I + 1>{}, 0, param...)...),
            param...),
          param...);
    }

    template <std::size_t... I>
    constexpr auto _bind(std::index_sequence<I...> i, CppParam... param) {
        return [i, ... p{std::forward<CppParam>(param)}, this]() {
            return _call(i, p...);
        };
    }

    template <std::size_t... CI, std::size_t... CppI>
    constexpr auto _raii(
      std::index_sequence<CI...> ci,
      std::index_sequence<CppI...>,
      adapted_function_raii_parameter_t<CppParam>... param) const noexcept {
        std::tuple<adapted_function_raii_parameter_t<CppParam>...> params{
          std::forward<decltype(param)>(param)...};
        return [ci, params, this]() mutable {
            _call(ci, std::move(std::get<CppI>(params))...);
        };
    }

public:
    basic_adapted_function(Api& api) noexcept
      : _api{api} {}

    [[nodiscard]] constexpr auto underlying() const noexcept -> decltype(auto) {
        return _api.*method;
    }

    explicit constexpr operator bool() const noexcept {
        return bool(underlying());
    }

    constexpr auto operator()(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return _call(S{}, std::forward<CppParam>(param)...);
    }

    [[nodiscard]] auto bind(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return _bind(S{}, std::forward<CppParam>(param)...);
    }

    [[nodiscard]] auto raii(
      adapted_function_raii_parameter_t<CppParam>... param) const noexcept {
        using CS = std::make_index_sequence<sizeof...(CParam)>;
        using CppS = std::make_index_sequence<sizeof...(CppParam)>;
        return eagine::finally(
          _raii(CS{}, CppS{}, std::forward<decltype(param)>(param)...));
    }

    auto fail() const noexcept {
        return RvMap{}(size_constant<0>{}, Ftw::template fail<CppRV>());
    }

    [[nodiscard]] constexpr auto api() const noexcept -> const auto& {
        return _api;
    }

    [[nodiscard]] constexpr auto api() noexcept -> auto& {
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
    combined_function(string_view name, Api& api)
      : _basic{name, api}
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

