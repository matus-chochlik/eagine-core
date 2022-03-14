/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_WRAP_HPP
#define EAGINE_C_API_WRAP_HPP

#include "c_api/constant.hpp"
#include "c_api/function.hpp"
#include "c_str.hpp"
#include "cleanup_group.hpp"
#include "enum_bitfield.hpp"
#include "handle.hpp"

namespace eagine {
//------------------------------------------------------------------------------
template <typename Api, typename ApiTraits, typename Tag>
class derived_c_api_function {
protected:
    template <typename RV, typename... Params, typename... Args>
    static constexpr auto _call(
      const c_api::unimplemented_function<ApiTraits, Tag, RV(Params...)>&,
      Args&&...) noexcept -> typename ApiTraits::template no_result<RV> {
        return {};
    }

    template <
      typename RV,
      typename... Params,
      typename... Args,
      RV (*Func)(Params...)>
    static constexpr auto _call(
      c_api::static_function<ApiTraits, Tag, RV(Params...), Func>& function,
      Args&&... args) noexcept ->
      typename ApiTraits::template result<RV> requires(!std::is_void_v<RV>) {
        return {std::move(function(std::forward<Args>(args)...))};
    }

    template <typename... Params, typename... Args, void (*Func)(Params...)>
    static constexpr auto _call(
      c_api::static_function<ApiTraits, Tag, void(Params...), Func>& function,
      Args&&... args) noexcept -> typename ApiTraits::template result<void> {
        function(std::forward<Args>(args)...);
        return {};
    }

    template <typename RV, typename... Params, typename... Args>
    static constexpr auto _call(
      c_api::dynamic_function<ApiTraits, Tag, RV(Params...)>& function,
      Args&&... args) noexcept -> typename ApiTraits::template opt_result<RV> {
        return {
          std::move(function(std::forward<Args>(args)...)), bool(function)};
    }

    template <typename... Params, typename... Args>
    static constexpr auto _call(
      c_api::dynamic_function<ApiTraits, Tag, void(Params...)>& function,
      Args&&... args) noexcept ->
      typename ApiTraits::template opt_result<void> {
        function(std::forward<Args>(args)...);
        return {bool(function)};
    }

    template <typename RV, typename... Params>
    static constexpr auto _fake(
      const c_api::unimplemented_function<ApiTraits, Tag, RV(Params...)>&,
      RV fallback) noexcept -> typename ApiTraits::template result<RV> {
        return {std::move(fallback)};
    }

    template <typename RV, typename... Params>
    static constexpr auto _fake(
      const c_api::unimplemented_function<ApiTraits, Tag, RV(Params...)>&) noexcept
      -> typename ApiTraits::template result<RV> {
        return {RV{}};
    }

    template <typename RV, typename... Params, RV (*Func)(Params...), typename F>
    static constexpr auto _fake(
      const c_api::static_function<ApiTraits, Tag, RV(Params...), Func>&,
      F&& fallback) noexcept -> typename ApiTraits::template result<RV> {
        return {std::forward<F>(fallback)};
    }

    template <typename RV, typename... Params, RV (*Func)(Params...)>
    static constexpr auto _fake(
      const c_api::static_function<ApiTraits, Tag, RV(Params...), Func>&) noexcept
      -> typename ApiTraits::template result<RV> {
        return {RV{}};
    }

    template <typename RV, typename... Params, typename F>
    static constexpr auto _fake(
      const c_api::dynamic_function<ApiTraits, Tag, RV(Params...)>&,
      F&& fallback) noexcept -> typename ApiTraits::template result<RV> {
        return {std::forward<F>(fallback)};
    }

    template <typename RV, typename... Params>
    static constexpr auto _fake(
      const c_api::dynamic_function<ApiTraits, Tag, RV(Params...)>&) noexcept ->
      typename ApiTraits::template result<RV> {
        return {RV{}};
    }

public:
    constexpr derived_c_api_function(
      const string_view name,
      ApiTraits&,
      Api& parent) noexcept
      : _name{name}
      , _parent{parent} {}

    constexpr auto name() const noexcept -> string_view {
        return _name;
    }

protected:
    constexpr auto api() const noexcept -> Api& {
        return _parent;
    }

private:
    const string_view _name{};
    Api& _parent;
};
//------------------------------------------------------------------------------
template <
  typename Api,
  typename ApiTraits,
  typename Tag,
  typename WrapperType,
  WrapperType Api::*Function>
class wrapped_c_api_function
  : public derived_c_api_function<Api, ApiTraits, Tag> {
    using base = derived_c_api_function<Api, ApiTraits, Tag>;

protected:
    template <typename... Args>
    constexpr auto _call(Args&&... args) const noexcept {
        return base::_call(this->api().*Function, std::forward<Args>(args)...);
    }

    template <typename F>
    constexpr auto _fake(F&& fallback) const noexcept {
        return base::_fake(this->api().*Function, std::forward<F>(fallback));
    }

    constexpr auto _fake() const noexcept {
        return base::_fake(this->api().*Function);
    }

    template <typename Arg>
    static constexpr auto _conv(Arg arg) noexcept -> Arg
      requires(std::is_scalar_v<Arg>) {
        return arg;
    }

    template <typename S, typename T, identifier_t L, identifier_t I>
    static constexpr auto _conv(const enum_class<S, T, L, I> value) noexcept {
        return T(value);
    }

    template <typename EC>
    static constexpr auto _conv(const enum_bitfield<EC> bits) noexcept {
        return _conv(bits._value);
    }

    template <typename Tg, typename T, T inv>
    static constexpr auto _conv(basic_handle<Tg, T, inv> hndl) noexcept {
        return T(hndl);
    }

    static constexpr auto _conv(const string_view str) noexcept {
        return c_str(str);
    }

    static constexpr auto _conv(memory::block blk) noexcept {
        return blk.data();
    }

    static constexpr auto _conv(const memory::const_block blk) noexcept {
        return blk.data();
    }

public:
    using base::base;

    constexpr auto fake_empty_c_str() const noexcept ->
      typename ApiTraits::template result<const char*> {
        return {static_cast<const char*>("")};
    }

    explicit constexpr operator bool() const noexcept {
        return bool((this->api()).*Function);
    }
};
//------------------------------------------------------------------------------

} // namespace eagine

#endif // EAGINE_C_API_WRAP_HPP
