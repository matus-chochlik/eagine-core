/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:reference;

import std;
import eagine.core.types;

namespace eagine::c_api {
//------------------------------------------------------------------------------
template <typename F, typename ApiWrapper, typename OpsWrapper, typename ConWrapper>
concept basic_api_reference_function_oca = requires(F f) {
    f(std::declval<const OpsWrapper>(),
      std::declval<const ConWrapper>(),
      std::declval<const ApiWrapper>());
};

template <typename F, typename ApiWrapper, typename OpsWrapper, typename ConWrapper>
concept basic_api_reference_function_oc = requires(F f) {
    f(std::declval<const OpsWrapper>(), std::declval<const ConWrapper>());
};

template <typename F, typename ApiWrapper, typename OpsWrapper, typename ConWrapper>
concept basic_api_reference_function_o =
  requires(F f) { f(std::declval<const OpsWrapper>()); };
//------------------------------------------------------------------------------
export template <
  typename ApiWrapper,
  typename OpsWrapper = std::tuple_element_t<0, ApiWrapper>,
  typename ConWrapper = std::tuple_element_t<1, ApiWrapper>>
class basic_api_reference : public optional_reference<ApiWrapper> {

public:
    using optional_reference<ApiWrapper>::optional_reference;

    constexpr auto and_then(
      basic_api_reference_function_oca<ApiWrapper, OpsWrapper, ConWrapper> auto&&
        function) const
      noexcept(noexcept(std::invoke(
        function,
        std::declval<const OpsWrapper>(),
        std::declval<const ConWrapper>(),
        std::declval<const ApiWrapper>()))) {
        using R = std::invoke_result_t<
          decltype(function),
          const OpsWrapper&,
          const ConWrapper&,
          const ApiWrapper&>;
        const auto invoke{[&, this] -> R {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            return decltype(function)(function)(operations, constants, api);
        }};
        if constexpr(std::convertible_to<R, bool>) {
            if(this->has_value()) [[likely]] {
                return tribool{invoke(), true};
            }
            return tribool{indeterminate};
        } else if constexpr(std::is_pointer_v<R> or std::is_reference_v<R>) {
            if(this->has_value()) [[likely]] {
                return optional_reference<R>{invoke()};
            }
            return optional_reference<R>{};
        } else if constexpr(not std::is_void_v<R>) {
            if(this->has_value()) [[likely]] {
                return optionally_valid<R>{invoke(), true};
            }
            return optionally_valid<R>{};
        } else {
            if(this->has_value()) [[likely]] {
                return invoke(), true;
            }
            return false;
        }
    }

    constexpr auto and_then(
      basic_api_reference_function_oc<ApiWrapper, OpsWrapper, ConWrapper> auto&&
        function) const
      noexcept(noexcept(std::invoke(
        function,
        std::declval<const OpsWrapper>(),
        std::declval<const ConWrapper>()))) {
        return and_then(
          [&](
            const OpsWrapper& operations,
            const ConWrapper& constants,
            const ApiWrapper&) { return function(operations, constants); });
    }

    constexpr auto and_then(
      basic_api_reference_function_o<ApiWrapper, OpsWrapper, ConWrapper> auto&&
        function) const
      noexcept(
        noexcept(std::invoke(function, std::declval<const OpsWrapper>()))) {
        return and_then([&](
                          const OpsWrapper& operations,
                          const ConWrapper&,
                          const ApiWrapper&) { return function(operations); });
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::c_api
