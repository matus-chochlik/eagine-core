/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:function;

import std;
import eagine.core.types;
import eagine.core.memory;
import :api_traits;
import :result;

namespace eagine::c_api {
//------------------------------------------------------------------------------
export template <typename ApiTraits, typename Tag, typename Signature>
using function_ptr =
  typename ApiTraits::template function_pointer<Tag, Signature>::type;

export template <typename ApiTraits, typename Tag, typename Signature>
class unimplemented_function;

/// @brief Wrapper for statically-linked C-API functions.
/// @tparam ApiTraits a policy class that customizes the linking of functions.
/// @tparam Tag a tag type that can be used in customization of some operations
///         on the function.
/// @tparam Signature the wrapped C-API function signature.
/// @ingroup c_api_wrap
/// @see dynamic_function
/// @see opt_function
export template <
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
export template <typename ApiTraits, typename Tag, typename Signature>
class dynamic_function;
//------------------------------------------------------------------------------
export template <typename Api, typename ApiTraits, typename Tag>
class derived_function;
//------------------------------------------------------------------------------
export template <bool isAvailable>
class function_base : public std::bool_constant<isAvailable> {
public:
    constexpr function_base(const string_view name) noexcept
      : _name{name} {}

    constexpr auto name() const noexcept -> string_view {
        return _name;
    }

private:
    const string_view _name{};
};
//------------------------------------------------------------------------------
export template <typename ApiTraits, typename Tag, typename RV, typename... Params>
class unimplemented_function<ApiTraits, Tag, RV(Params...)>
  : public function_base<false> {
    using base = function_base<false>;

public:
    using signature = RV(Params...);

    template <typename Api>
    constexpr unimplemented_function(const string_view name, Api&)
      : base(name) {}

    template <typename... Args>
    constexpr auto operator()(Args&&...) const noexcept -> RV
        requires(sizeof...(Params) == sizeof...(Args))
    {
        return ApiTraits::fallback(Tag(), std::type_identity<RV>());
    }
};
//------------------------------------------------------------------------------
/// @brief Wrapper for statically-linked C-API functions.
/// @ingroup c_api_wrap
/// @see dynamic_function
/// @see opt_function
export template <
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
    constexpr static_function(const string_view name, Api&)
      : base(name) {}

    /// @brief Calls the wrapped function.
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const noexcept -> RV
        requires(sizeof...(Params) == sizeof...(Args))
    {
        return ApiTraits::call_static(
          Tag(), function, std::forward<Args>(args)...);
    }
};
//------------------------------------------------------------------------------
/// @brief Wrapper for dynamically -linked C-API functions.
/// @ingroup c_api_wrap
/// @see static_function
/// @see opt_function
export template <typename ApiTraits, typename Tag, typename RV, typename... Params>
class dynamic_function<ApiTraits, Tag, RV(Params...)>
  : public function_base<true> {

    using base = function_base<true>;
    using function_pointer = function_ptr<ApiTraits, Tag, RV(Params...)>;

public:
    /// @brief Alias for the wrapped function type.
    using signature = RV(Params...);

    template <typename Api>
    constexpr dynamic_function(const string_view name, Api& api)
      : base(name)
      , _function{api.traits().link_function(
          api,
          Tag(),
          name,
          std::type_identity<RV(Params...)>())} {}

    /// @brief Indicates if the function is linked (and can be used).
    constexpr explicit operator bool() const noexcept {
        return bool(_function);
    }

    /// @brief Calls the wrapped function.
    template <typename... Args>
    constexpr auto operator()(Args&&... args) const noexcept -> RV
        requires(sizeof...(Params) == sizeof...(Args))
    {
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
export template <
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
export template <typename W>
struct function_traits;

export template <typename ApiTraits, typename Tag, typename RV, typename... P>
struct function_traits<unimplemented_function<ApiTraits, Tag, RV(P...)>> {
    using api_traits = ApiTraits;

    template <typename T = RV>
    using result_type = typename ApiTraits::template no_result<T>;

    template <typename... Args>
    static constexpr auto call(
      const c_api::unimplemented_function<ApiTraits, Tag, RV(P...)>&,
      Args&&...) noexcept -> result_type<RV> {
        return {};
    }

    template <typename R = RV>
    static constexpr auto fake(const R&) noexcept -> result_type<R> {
        return {};
    }

    template <typename R = RV>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <typename ApiTraits, typename Tag, typename... P>
struct function_traits<unimplemented_function<ApiTraits, Tag, void(P...)>> {
    using api_traits = ApiTraits;

    template <typename T = void>
    using result_type = typename ApiTraits::template no_result<T>;

    template <typename... Args>
    static constexpr auto call(
      const c_api::unimplemented_function<ApiTraits, Tag, void(P...)>&,
      Args&&...) noexcept -> result_type<void> {
        return {};
    }

    template <typename R = void>
    static constexpr auto fake() noexcept -> result_type<R> {
        return {};
    }

    template <typename R = void>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <
  typename ApiTraits,
  typename Tag,
  typename RV,
  typename... P,
  auto f>
struct function_traits<static_function<ApiTraits, Tag, RV(P...), f>> {
    using api_traits = ApiTraits;

    template <typename T = RV>
    using result_type = typename ApiTraits::template result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::static_function<ApiTraits, Tag, RV(P...), f>& function,
      Args&&... args) noexcept -> result_type<RV> {
        return {function(std::forward<Args>(args)...)};
    }

    template <typename R = RV>
    static constexpr auto fake(R res) noexcept -> result_type<R> {
        return {std::move(res)};
    }

    template <typename R = RV>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <typename ApiTraits, typename Tag, typename... P, auto f>
struct function_traits<static_function<ApiTraits, Tag, void(P...), f>> {
    using api_traits = ApiTraits;

    template <typename T = void>
    using result_type = typename ApiTraits::template result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::static_function<ApiTraits, Tag, void(P...), f>& function,
      Args&&... args) noexcept -> result_type<void> {
        function(std::forward<Args>(args)...);
        return {};
    }

    template <typename R = void>
    static constexpr auto fake() noexcept -> result_type<R> {
        return {};
    }

    template <typename R = void>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <typename ApiTraits, typename Tag, typename RV, typename... P>
struct function_traits<dynamic_function<ApiTraits, Tag, RV(P...)>> {
    using api_traits = ApiTraits;

    template <typename T = RV>
    using result_type = typename ApiTraits::template opt_result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::dynamic_function<ApiTraits, Tag, RV(P...)>& function,
      Args&&... args) noexcept -> result_type<RV> {
        return {function(std::forward<Args>(args)...), bool(function)};
    }

    template <typename R = RV>
    static constexpr auto fake(R res) noexcept -> result_type<R> {
        return {std::move(res)};
    }

    template <typename R = RV>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <typename ApiTraits, typename Tag, typename... P>
struct function_traits<dynamic_function<ApiTraits, Tag, void(P...)>> {
    using api_traits = ApiTraits;

    template <typename T = void>
    using result_type = typename ApiTraits::template opt_result<T>;

    template <typename... Args>
    static constexpr auto call(
      c_api::dynamic_function<ApiTraits, Tag, void(P...)>& function,
      Args&&... args) noexcept -> result_type<void> {
        function(std::forward<Args>(args)...);
        return {bool(function)};
    }

    template <typename R = void>
    static constexpr auto fake() noexcept -> result_type<R> {
        return {};
    }

    template <typename R = void>
    static constexpr auto fail() noexcept ->
      typename ApiTraits::template opt_result<R> {
        return {};
    }
};

export template <typename T, typename W>
using function_result_t = typename function_traits<W>::template result_type<T>;

export template <typename T, typename W>
using function_api_traits_t = typename function_traits<W>::api_traits;
//------------------------------------------------------------------------------
} // namespace eagine::c_api

