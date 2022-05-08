/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_ADAPTED_FUNCTION_HPP
#define EAGINE_C_API_ADAPTED_FUNCTION_HPP

#include "../cleanup_group.hpp"
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

template <typename CppSignature, typename RvArgMap, typename Ftw>
struct adapted_function_utils;

template <typename CppRv, typename... CppParam, typename RvArgMap, typename Ftw>
struct adapted_function_utils<CppRv(CppParam...), RvArgMap, Ftw> {

    template <typename Api, typename Wrapped, std::size_t... I>
    static constexpr auto call(
      Api& api,
      Wrapped& func,
      std::index_sequence<I...>,
      CppParam... param) noexcept {
        RvArgMap map{};
        return api.check_result(
          map(
            size_constant<0>{},
            api,
            Ftw::call(func, map(size_constant<I + 1>{}, api, 0, param...)...),
            param...),
          param...);
    }

    template <typename Api, typename Wrapped, std::size_t... CI, std::size_t... CppI>
    static constexpr auto call(
      Api& api,
      Wrapped& func,
      std::index_sequence<CI...> s,
      std::index_sequence<CppI...>,
      std::tuple<CppParam...> params) noexcept {
        return call(api, func, s, std::move(std::get<CppI>(params))...);
    }

    template <typename Api, typename Wrapped, std::size_t... I>
    static constexpr auto bind(
      Api& api,
      Wrapped& func,
      std::index_sequence<I...> i,
      CppParam... param) noexcept {
        return [&api, &func, i, ... p{std::forward<CppParam>(param)}]() {
            return call(api, func, i, p...);
        };
    }

    template <typename Api, typename Wrapped, std::size_t... CI, std::size_t... CppI>
    static constexpr auto raii(
      Api& api,
      Wrapped& func,
      std::index_sequence<CI...> ci,
      std::index_sequence<CppI...>,
      adapted_function_raii_parameter_t<CppParam>... param) noexcept {
        std::tuple<adapted_function_raii_parameter_t<CppParam>...> params{
          std::forward<decltype(param)>(param)...};
        return [&api, &func, ci, params]() mutable {
            (void)params;
            call(api, func, ci, std::move(std::get<CppI>(params))...);
        };
    }

    template <typename Api>
    auto fail(Api& api) const noexcept {
        return RvArgMap{}(size_constant<0>{}, api, Ftw::template fail<CppRv>());
    }
};

template <
  typename Api,
  auto method,
  typename CSignature,
  typename CppSignature = CSignature,
  typename RvArgMap = trivial_map>
class basic_adapted_function;

template <
  typename Api,
  auto method,
  typename CRV,
  typename... CParam,
  typename CppRV,
  typename... CppParam,
  typename RvArgMap>
class basic_adapted_function<
  Api,
  method,
  CRV(CParam...),
  CppRV(CppParam...),
  RvArgMap> {
    [[no_unique_address]] Api& _api;
    [[no_unique_address]] adapted_function_utils<
      CppRV(CppParam...),
      RvArgMap,
      function_traits<method_wrapper_t<method>>>
      _utils;

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
        return _utils.call(
          _api, _api.*method, S{}, std::forward<CppParam>(param)...);
    }

    constexpr auto operator[](std::tuple<CppParam...> params) const noexcept {
        using CS = std::make_index_sequence<sizeof...(CParam)>;
        using CppS = std::make_index_sequence<sizeof...(CppParam)>;
        return _utils.call(_api, _api.*method, CS{}, CppS{}, std::move(params));
    }

    [[nodiscard]] auto bind(CppParam... param) const noexcept {
        using S = std::make_index_sequence<sizeof...(CParam)>;
        return _utils.bind(
          _api, _api.*method, S{}, std::forward<CppParam>(param)...);
    }

    [[nodiscard]] auto raii(
      adapted_function_raii_parameter_t<CppParam>... param) const noexcept {
        using CS = std::make_index_sequence<sizeof...(CParam)>;
        using CppS = std::make_index_sequence<sizeof...(CppParam)>;
        return eagine::finally(_utils.raii(
          _api,
          _api.*method,
          CS{},
          CppS{},
          std::forward<decltype(param)>(param)...));
    }

    auto later_by(
      cleanup_group& cleanup,
      adapted_function_raii_parameter_t<CppParam>... param) const noexcept
      -> decltype(auto) {
        using CS = std::make_index_sequence<sizeof...(CParam)>;
        using CppS = std::make_index_sequence<sizeof...(CppParam)>;

        return cleanup.add_ret(_utils.raii(
          _api,
          _api.*method,
          CS{},
          CppS{},
          std::forward<decltype(param)>(param)...));
    }

    [[nodiscard]] auto fail() const noexcept {
        return _utils.fail(_api);
    }

    [[nodiscard]] constexpr auto api() const noexcept -> const auto& {
        return _api;
    }

    [[nodiscard]] constexpr auto api() noexcept -> auto& {
        return _api;
    }
};

template <typename Result, typename Remain>
struct get_transformed_signature;

template <typename Rv, typename... P>
struct get_transformed_signature<Rv(P...), mp_list<>> {
    using type = Rv(P...);
};

template <typename Rv, typename... P>
struct get_transformed_signature<returned<Rv>(P...), mp_list<>> {
    using type = Rv(P...);
};

template <typename Rv, typename... P>
struct get_transformed_signature<collapsed<Rv>(P...), mp_list<>> {
    using type = void(P...);
};

template <typename Rv, typename... P, typename... T>
struct get_transformed_signature<Rv(P...), mp_list<skipped, T...>>
  : get_transformed_signature<Rv(P...), mp_list<T...>> {};

template <typename Rv, typename... P, typename... T>
struct get_transformed_signature<Rv(P...), mp_list<defaulted, T...>>
  : get_transformed_signature<Rv(P...), mp_list<T...>> {};

template <typename Rv, typename... P, auto value, typename... T>
struct get_transformed_signature<Rv(P...), mp_list<substituted<value>, T...>>
  : get_transformed_signature<Rv(P...), mp_list<T...>> {};

template <typename Rv, typename... P, typename... T>
struct get_transformed_signature<returned<Rv>(P...), mp_list<returned<Rv>, T...>>
  : get_transformed_signature<returned<Rv>(P...), mp_list<T...>> {};

template <typename Rv, typename... P, typename H, typename... T>
struct get_transformed_signature<Rv(P...), mp_list<H, T...>>
  : get_transformed_signature<Rv(P..., H), mp_list<T...>> {};

template <typename CppSignature>
struct transform_signature;

template <typename CppSignature>
using transform_signature_t = typename transform_signature<CppSignature>::type;

template <typename Rv, typename... P>
struct transform_signature<Rv(P...)>
  : get_transformed_signature<Rv(), mp_list<P...>> {};

template <typename... BasicAdaptedFunction>
class combined;

template <
  typename Api,
  auto... methods,
  typename... CSignatures,
  typename... CppSignatures,
  typename... RvArgMaps>
class combined<
  basic_adapted_function<Api, methods, CSignatures, CppSignatures, RvArgMaps>...>
  : public basic_adapted_function<
      Api,
      methods,
      CSignatures,
      CppSignatures,
      RvArgMaps>... {
public:
    constexpr combined(Api& api) noexcept
      : basic_adapted_function<Api, methods, CSignatures, CppSignatures, RvArgMaps>{
          api}... {}

    constexpr explicit operator bool() const noexcept {
        return (
          ... && basic_adapted_function<
                   Api,
                   methods,
                   CSignatures,
                   CppSignatures,
                   RvArgMaps>::operator bool());
    }

    using basic_adapted_function<
      Api,
      methods,
      CSignatures,
      CppSignatures,
      RvArgMaps>::operator()...;

    using basic_adapted_function<
      Api,
      methods,
      CSignatures,
      CppSignatures,
      RvArgMaps>::operator[]...;

    using basic_adapted_function<
      Api,
      methods,
      CSignatures,
      CppSignatures,
      RvArgMaps>::raii...;

    using basic_adapted_function<
      Api,
      methods,
      CSignatures,
      CppSignatures,
      RvArgMaps>::later_by...;
};

template <
  auto method,
  typename CppSignature = method_signature_t<method>,
  typename RvArgMap = make_map_t<method_signature_t<method>, CppSignature>>
using adapted_function = basic_adapted_function<
  method_api_t<method>,
  method,
  method_signature_t<method>,
  transform_signature_t<CppSignature>,
  RvArgMap>;

template <
  typename Api,
  typename ApiTraits,
  typename Tag,
  typename CSignature,
  typename CppSignature,
  typename RvArgMap,
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
      RvArgMap>
      _adapted;
};

} // namespace eagine::c_api

#endif

