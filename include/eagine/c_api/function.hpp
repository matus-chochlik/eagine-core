/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_FUNCTION_HPP
#define EAGINE_C_API_FUNCTION_HPP

#include "../valid_if/always.hpp"
#include "../valid_if/never.hpp"
#include "result.hpp"

namespace eagine::c_api {
//------------------------------------------------------------------------------
template <typename ApiTraits, typename Tag, typename Signature>
using function_ptr =
  typename ApiTraits::template function_pointer<Tag, Signature>::type;

template <typename ApiTraits, typename Tag, typename Signature>
class unimplemented_function;

/// @brief Wrapper for statically-linked C-API functions.
/// @tparam ApiTraits a policy class that customizes the linking of functions.
/// @tparam Tag a tag type that can be used in customization of some operations
///         on the function.
/// @tparam Signature the wrapped C-API function signature.
/// @ingroup c_api_wrap
/// @see dynamic_function
/// @see opt_function
template <
  typename ApiTraits,
  typename Tag,
  typename Signature,
  function_ptr<ApiTraits, Tag, Signature> function>
class static_function;
//------------------------------------------------------------------------------
/// @brief Wrapper for dynamically-linked C-API functions.
/// @tparam ApiTraits a policy class that customizes the linking of functions.
/// @tparam Tag a tag type that can be used in custiomization of some operations
///         on the function.
/// @tparam Signature the wrapped C-API function signature.
/// @ingroup c_api_wrap
/// @see static_function
/// @see opt_function
template <typename ApiTraits, typename Tag, typename Signature>
class dynamic_function;
//------------------------------------------------------------------------------
template <typename Api, typename ApiTraits, typename Tag>
class derived_function;
//------------------------------------------------------------------------------
/// @brief Policy class customizing C-API constant value loading and function linking.
/// @ingroup c_api_wrap
struct default_traits {
    template <typename Tag, typename Signature>
    using function_pointer = std::add_pointer<Signature>;

    template <typename Api, typename Type>
    auto load_constant(Api&, const string_view, const type_identity<Type>)
      -> std::tuple<Type, bool> {
        return {{}, false};
    }

    template <typename Api, typename Tag, typename Signature>
    auto link_function(
      Api&,
      const Tag,
      const string_view,
      const type_identity<Signature>) -> std::add_pointer_t<Signature> {
        return nullptr;
    }

    template <typename Tag, typename RV>
    static constexpr auto fallback(const Tag, const type_identity<RV>) -> RV {
        return {};
    }

    template <typename Tag>
    static constexpr void fallback(const Tag, const type_identity<void>) {}

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_static(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> RV {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, type_identity<RV>());
    }

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_dynamic(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> RV {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, type_identity<RV>());
    }

    template <typename RV>
    using no_result = never_valid<RV>;

    template <typename RV>
    using opt_result = optionally_valid<RV>;

    template <typename RV>
    using result = always_valid<RV>;
};
//------------------------------------------------------------------------------
template <bool isAvailable>
class function_base : public bool_constant<isAvailable> {
public:
    constexpr function_base(const string_view name) noexcept
      : _name{name} {}

    constexpr explicit operator bool() const noexcept {
        return isAvailable;
    }

    constexpr auto name() const noexcept -> string_view {
        return _name;
    }

private:
    const string_view _name{};
};
//------------------------------------------------------------------------------
template <typename ApiTraits, typename Tag, typename RV, typename... Params>
class unimplemented_function<ApiTraits, Tag, RV(Params...)>
  : public function_base<false> {
    using base = function_base<false>;

public:
    using signature = RV(Params...);

    template <typename Api>
    constexpr unimplemented_function(
      const string_view name,
      const ApiTraits&,
      Api&)
      : base(name) {}

    template <typename... Args>
    constexpr auto operator()(Args&&...) const noexcept -> RV
      requires(sizeof...(Params) == sizeof...(Args)) {
        return ApiTraits::fallback(Tag(), type_identity<RV>());
    }
};
//------------------------------------------------------------------------------
/// @brief Wrapper for statically-linked C-API functions.
/// @ingroup c_api_wrap
/// @see dynamic_function
/// @see opt_function
template <
  typename ApiTraits,
  typename Tag,
  typename RV,
  typename... Params,
  function_ptr<ApiTraits, Tag, RV(Params...)> function>
class static_function<ApiTraits, Tag, RV(Params...), function>
  : public function_base<true> {
    using base = function_base<true>;

public:
    /// @brief Alias for the wrapped function type.
    using signature = RV(Params...);

    template <typename Api>
    constexpr static_function(const string_view name, const ApiTraits&, Api&)
      : base(name) {}

    /// @brief Calls the wrapped function.
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const noexcept -> RV
      requires(sizeof...(Params) == sizeof...(Args)) {
        return ApiTraits::call_static(
          Tag(), function, std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
/// @brief Wrapper for dynamically -linked C-API functions.
/// @ingroup c_api_wrap
/// @see static_function
/// @see opt_function
template <typename ApiTraits, typename Tag, typename RV, typename... Params>
class dynamic_function<ApiTraits, Tag, RV(Params...)>
  : public function_base<true> {

    using base = function_base<true>;
    using function_pointer = function_ptr<ApiTraits, Tag, RV(Params...)>;

public:
    /// @brief Alias for the wrapped function type.
    using signature = RV(Params...);

    template <typename Api>
    constexpr dynamic_function(
      const string_view name,
      ApiTraits& traits,
      Api& api)
      : base(name)
      , _function{traits.link_function(
          api,
          Tag(),
          name,
          type_identity<RV(Params...)>())} {}

    /// @brief Indicates if the function is linked (and can be used).
    constexpr explicit operator bool() const noexcept {
        return bool(_function);
    }

    /// @brief Calls the wrapped function.
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const noexcept -> RV
      requires(sizeof...(Params) == sizeof...(Args)) {
        return ApiTraits::call_dynamic(
          Tag(), _function, std::forward<Args>(args)...);
    }

private:
    function_pointer _function{nullptr};
};
//------------------------------------------------------------------------------
/// @brief Template alias used for switching between static and dynamic function.
/// @tparam ApiTraits a policy class that customizes the linking of functions.
/// @tparam Tag a tag type that can be used in custiomization of some operations
///         on the function.
/// @tparam Signature the wrapped C-API function signature.
/// @ingroup c_api_wrap
///
/// @see static_function
/// @see dynamic_function
/// @see mp_list
///
/// C-API constants can be either known at compile time or loaded dynamically
/// by using some API function(s).
template <
  typename ApiTraits,
  typename Tag,
  typename Signature,
  function_ptr<ApiTraits, Tag, Signature> function,
  bool isAvailable,
  bool isStatic>
using opt_function = std::conditional_t<
  isAvailable,
  std::conditional_t<
    isStatic,
    static_function<ApiTraits, Tag, Signature, function>,
    dynamic_function<ApiTraits, Tag, Signature>>,
  unimplemented_function<ApiTraits, Tag, Signature>>;
//------------------------------------------------------------------------------
template <typename W>
struct function_traits;

template <typename ApiTraits, typename Tag, typename RV, typename... P>
struct function_traits<unimplemented_function<ApiTraits, Tag, RV(P...)>> {
    template <typename T = RV>
    using type = typename ApiTraits::template opt_result<T>;

    template <typename... Args>
    static constexpr auto call(
      const c_api::unimplemented_function<ApiTraits, Tag, RV(P...)>&,
      Args&&...) noexcept -> typename ApiTraits::template no_result<RV> {
        return {};
    }
};

template <typename ApiTraits, typename Tag, typename RV, typename... P, auto f>
struct function_traits<static_function<ApiTraits, Tag, RV(P...), f>> {
    template <typename T = RV>
    using type = typename ApiTraits::template result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::static_function<ApiTraits, Tag, RV(P...), f>& function,
      Args&&... args) noexcept -> typename ApiTraits::template result<RV> {
        return {std::move(function(std::forward<Args>(args)...))};
    }
};

template <typename ApiTraits, typename Tag, typename... P, auto f>
struct function_traits<static_function<ApiTraits, Tag, void(P...), f>> {
    template <typename T = void>
    using type = typename ApiTraits::template result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::static_function<ApiTraits, Tag, void(P...), f>& function,
      Args&&... args) noexcept -> typename ApiTraits::template result<void> {
        function(std::forward<Args>(args)...);
        return {};
    }
};

template <typename ApiTraits, typename Tag, typename RV, typename... P>
struct function_traits<dynamic_function<ApiTraits, Tag, RV(P...)>> {
    template <typename T = RV>
    using type = typename ApiTraits::template opt_result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::dynamic_function<ApiTraits, Tag, RV(P...)>& function,
      Args&&... args) noexcept -> typename ApiTraits::template opt_result<RV> {
        return {
          std::move(function(std::forward<Args>(args)...)), bool(function)};
    }
};

template <typename ApiTraits, typename Tag, typename... P>
struct function_traits<dynamic_function<ApiTraits, Tag, void(P...)>> {
    template <typename T = void>
    using type = typename ApiTraits::template opt_result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::dynamic_function<ApiTraits, Tag, void(P...)>& function,
      Args&&... args) noexcept ->
      typename ApiTraits::template opt_result<void> {
        function(std::forward<Args>(args)...);
        return {bool(function)};
    }
};

template <typename T, typename W>
using function_result_t = typename function_traits<W>::template type<T>;
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif
