/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:reference;

import std;
import eagine.core.types;

namespace eagine::c_api {
//------------------------------------------------------------------------------
export template <
  typename ApiWrapper,
  typename OpsWrapper = std::tuple_element_t<0, ApiWrapper>,
  typename ValWrapper = std::tuple_element_t<1, ApiWrapper>>
class basic_api_reference : public optional_reference<ApiWrapper> {
    template <bool isNoexcept, typename R, typename F, typename... Args>
    auto _then(F function, Args&&... args) const noexcept(isNoexcept) {
        const auto invoke{[&, this] -> decltype(auto) {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            return function(operations, constants, api);
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

    template <bool isNoexcept, typename C, typename F, typename... Args>
    auto _then_member(C& obj, F member_function, Args&&... args) const
      noexcept(isNoexcept) {
        using R = std::invoke_result_t<
          F,
          C&,
          const OpsWrapper&,
          const ValWrapper&,
          const ApiWrapper,
          Args...>;
        return _then<isNoexcept, R>(
          [&](auto& operations, auto& constants, auto& api) {
              return (obj.*member_function)(
                operations, constants, api, std::forward<Args>(args)...);
          },
          std::forward<Args>(args)...);
    }

    template <typename Class, typename Sig, bool isConst, bool isNoexcept>
    struct _then_function_helper;

    template <typename Class, typename Result, typename... Params>
    struct _then_function_helper<Class, Result(Params...), false, false>
      : std::type_identity<Result (Class::*)(
          const OpsWrapper&,
          const ValWrapper&,
          const ApiWrapper&,
          Params...)> {};

    template <typename Class, typename Result, typename... Params>
    struct _then_function_helper<Class, Result(Params...), false, true>
      : std::type_identity<Result (Class::*)(
          const OpsWrapper&,
          const ValWrapper&,
          const ApiWrapper&,
          Params...) noexcept> {};

    template <typename Class, typename Result, typename... Params>
    struct _then_function_helper<Class, Result(Params...), true, false>
      : std::type_identity<Result (Class::*)(
          const OpsWrapper&,
          const ValWrapper&,
          const ApiWrapper&,
          Params...) const> {};

    template <typename Class, typename Result, typename... Params>
    struct _then_function_helper<Class, Result(Params...), true, true>
      : std::type_identity<Result (Class::*)(
          const OpsWrapper&,
          const ValWrapper&,
          const ApiWrapper&,
          Params...) const noexcept> {};

public:
    using optional_reference<ApiWrapper>::optional_reference;

    template <typename Class, typename Sig, bool isConst, bool isNoexcept>
    using then_function =
      typename _then_function_helper<Class, Sig, isConst, isNoexcept>::type;

    template <typename Function>
    auto then(Function&& function) const {
        return _then<false, void>(std::forward<Function>(function));
    }

    template <typename Sig = void(), typename C, typename... Args>
    auto then(
      C* obj,
      then_function<C, Sig, false, false> member_function,
      Args&&... args) const {
        return _then_member<false>(
          *obj, member_function, std::forward<Args>(args)...);
    }

    template <typename Sig = void(), typename C, typename... Args>
    auto then(
      C* obj,
      then_function<C, Sig, false, true> member_function,
      Args&&... args) const noexcept {
        return _then_member<true>(
          *obj, member_function, std::forward<Args>(args)...);
    }

    template <typename Sig = void(), typename C, typename... Args>
    auto then(
      const C* obj,
      then_function<C, Sig, true, false> member_function,
      Args&&... args) const {
        return _then_member<false>(
          *obj, member_function, std::forward<Args>(args)...);
    }

    template <typename Sig = void(), typename C, typename... Args>
    auto then(
      const C* obj,
      then_function<C, Sig, true, true> member_function,
      Args&&... args) const noexcept {
        return _then_member<true>(
          *obj, member_function, std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::c_api
